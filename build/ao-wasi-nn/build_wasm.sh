emcc -O3 -g2 \
     -s STANDALONE_WASM=1 \
     -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
     -s EXPORTED_FUNCTIONS=[\"_lib_main\"] \
     -Wl,--no-entry \
     -o process.wasm \
     wasi_nn.c

# /opt/wasi-sdk/bin/clang \
#     --target=wasm32-wasi \
#     -DNN_LOG_LEVEL=1 \
#     -Wl,--allow-undefined \
#     -I./wasi_nn_types.h \
#     -I./wasi_nn_api.h \
#     -I./wasi_nn.h \
#     -o process.wasm \
#     wasi_nn.c
