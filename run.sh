#!/bin/bash
# cmake --build build -- -DCMAKE_EXPORT_COMPILE_COMMANDS=1 &&

mkdir -p build &&
cmake -B./build -S./ &&
cmake --build ./build &&
./build/edi
