.PHONY: local-model-server build-ao-llama build-llama clean

# Variables
SCRIPT_DIR := $(shell pwd)
LLAMA_CPP_DIR := $(SCRIPT_DIR)/build/llamacpp
AO_LLAMA_DIR := $(SCRIPT_DIR)/build/ao-llama
PROCESS_DIR := $(SCRIPT_DIR)/aos/process
LIBS_DIR := $(PROCESS_DIR)/libs
AO_IMAGE := p3rmaw3b/ao:0.1.4
EMXX_CFLAGS := -sMEMORY64=1 -O3 -msimd128 -fno-rtti -Wno-experimental

local-model-server:
	docker run --rm -p 3001:80 -v ./models:/usr/share/nginx/html:ro nginx

clean:
	rm -rf $(LLAMA_CPP_DIR)
	rm -rf libs
	rm -rf $(AO_LLAMA_DIR)/*.so

build-llama: clean
	# Clone llama.cpp
	git clone https://github.com/ggerganov/llama.cpp.git $(LLAMA_CPP_DIR)
	cd $(LLAMA_CPP_DIR) && git checkout tags/b3233 -b b3233
	
	# Patch llama.cpp to remove alignment asserts
	sed -i.bak 's/#define ggml_assert_aligned.*/#define ggml_assert_aligned\(ptr\)/g' $(LLAMA_CPP_DIR)/ggml.c
	sed -i.bak '/.*GGML_ASSERT.*GGML_MEM_ALIGN == 0.*/d' $(LLAMA_CPP_DIR)/ggml.c
	
	# Build llama.cpp into a static library with emscripten
	docker run -v $(LLAMA_CPP_DIR):/llamacpp $(AO_IMAGE) sh -c \
		"cd /llamacpp && emcmake cmake -DCMAKE_CXX_FLAGS='$(EMXX_CFLAGS)' -S . -B . -DLLAMA_BUILD_EXAMPLES=OFF"
	
	docker run -v $(LLAMA_CPP_DIR):/llamacpp $(AO_IMAGE) sh -c \
		"cd /llamacpp && emmake make llama common EMCC_CFLAGS='$(EMXX_CFLAGS)' -j 8"
	
	# Build ao-llama components
	$(MAKE) build-ao-llama
	
	# Copy llama.cpp to the libs directory
	mkdir -p $(LIBS_DIR)/llamacpp/common
	cp $(LLAMA_CPP_DIR)/libllama.a $(LIBS_DIR)/llamacpp/libllama.a
	cp $(LLAMA_CPP_DIR)/common/libcommon.a $(LIBS_DIR)/llamacpp/common/libcommon.a
	
	# Copy ao-llama to the libs directory
	mkdir -p $(LIBS_DIR)/ao-llama
	cp $(AO_LLAMA_DIR)/libaollama.so $(LIBS_DIR)/ao-llama/libaollama.so
	cp $(AO_LLAMA_DIR)/libaostream.so $(LIBS_DIR)/ao-llama/libaostream.so
	cp $(AO_LLAMA_DIR)/Llama.lua $(PROCESS_DIR)/Llama.lua
	
	# Copy libs to script directory
	cp -r $(LIBS_DIR) $(SCRIPT_DIR)/libs
	
	# Remove .so files
	rm -rf $(AO_LLAMA_DIR)/*.so
	
	# Copy config.yml to the process directory
	cp $(SCRIPT_DIR)/config.yml $(PROCESS_DIR)/config.yml
	
	# Build the process module
	cd $(PROCESS_DIR) && \
	docker run -e DEBUG=1 --platform linux/amd64 -v ./:/src $(AO_IMAGE) ao-build-module
	
	# Copy the process module to the test-llm directory
	cp $(PROCESS_DIR)/process.wasm $(SCRIPT_DIR)/tests/aos-llama.wasm
	
	# If the process.js file exists, copy it to the test-llm directory
	[ -f "$(PROCESS_DIR)/process.js" ] && cp $(PROCESS_DIR)/process.js $(SCRIPT_DIR)/tests/process.js || true

build-ao-llama:
	docker run -v $(LLAMA_CPP_DIR):/llamacpp -v $(AO_LLAMA_DIR):/ao-llama $(AO_IMAGE) sh -c \
		"cd /ao-llama && ./build.sh"
