emcc -O3 -g2 \
     -s STANDALONE_WASM=1 \
     -s PURE_WASI=1 \
     -s ALLOW_MEMORY_GROWTH=1 \
     -s ALLOW_TABLE_GROWTH=1 \
     -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
     -s ALLOW_UNIMPLEMENTED_SYSCALLS=1 \
     -s EXPORTED_FUNCTIONS=[\"_lib_main\",\"_malloc\",\"_free\"] \
     -Wl,--no-entry \
     -o process.wasm \
     wasi_nn.c
