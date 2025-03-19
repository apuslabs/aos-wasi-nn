emcc -sMEMORY64=1 -msimd128 -O3 -fno-rtti -fno-exceptions -Wno-experimental -c wasi_nn.c -o wasi_nn.o -I/llamacpp/ -I/llamacpp/common -I/lua-5.3.4/src
emcc -sMEMORY64=1 -msimd128 -O3 -fno-rtti -fno-exceptions -Wno-experimental -c wasi_nn_bindings.c -o wasi_nn_bindings.o -I/llamacpp/ -I/llamacpp/common -I/lua-5.3.4/src

emar rcs libaowasinn.so wasi_nn.o wasi_nn_bindings.o

rm wasi_nn.o wasi_nn_bindings.o

