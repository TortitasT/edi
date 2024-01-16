#!/bin/bash

mkdir -p build &&
cmake -B./build -S./ &&
cmake --build ./build &&
valgrind --leak-check=full -s ./build/edi
