#!/usr/bin/env sh

cmake -B./build-x86-64-mingw-win -DCMAKE_TOOLCHAIN_FILE=$PWD/mingw-w64-wsl.cmake -DUSE_GLAD=1
cmake --build ./build-x86-64-mingw-win
