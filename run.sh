#!/bin/bash
# cmake --build build -- -DCMAKE_EXPORT_COMPILE_COMMANDS=1 &&

function compile {
  mkdir -p build &&
  clang ./bin2h/main.c -o ./bin2h/bin2h &&
  ./bin2h/bin2h -i ./assets/fonts/GoMonoNerdFont-Regular.ttf -o ./include/font.h -n FONT_DATA &&
  cmake -B./build -S./ &&
  cmake --build ./build
}

function cleanup {
    rm -rf build
    rm -r bin2h/bin2h
}

if [ "$1" == "clean" ]; then
    cleanup
    exit 0
fi

if [ "$1" == "compile" ]; then
    compile
    exit 0
fi

if [ "$1" == "run" ]; then
    compile
    ./build/edi
    exit 0
fi

if [ "$1" == "debug" ]; then
    compile
    gdb ./build/edi
    exit 0
fi

if [ "$1" == "valgrind" ]; then
    compile
    valgrind --leak-check=full --show-leak-kinds=all ./build/edi
    exit 0
fi
