emcc -sMEMORY64=1 -msimd128 -O3 -fno-rtti -fno-exceptions -Wno-experimental -c lsqlite3.c -o sqlite3-bindings.o -I/lua-5.3.4/src
emcc -sMEMORY64=1 -msimd128 -O3 -fno-rtti -fno-exceptions -Wno-experimental -c sqlite3.c -o sqlite3.o -I/lua-5.3.4/src
emar rcs libsqlite3.so sqlite3.o sqlite3-bindings.o
rm sqlite3.o sqlite3-bindings.o