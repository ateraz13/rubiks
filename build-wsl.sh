#!/usr/bin/env bash

build_dir=./build-x86-64-linux
do_clean_build=0
toolchain=""

for x in "$@" ; do
    if [ "$x" = "--clean-build" ] ; then
        do_clean_build=1
    fi
done

if [[ ! -d "$build_dir" ||  "$do_clean_build" -eq 1 ]]; then
    rm -R "$build_dir"
    if [ "$toolchain" != "" ] ; then
        cmake -B"$build_dir" -DCMAKE_TOOLCHAIN_FILE=$PWD/mingw-w64-wsl.cmake -DUSE_GLAD=1
    else
        cmake -B"$build_dir" -DUSE_GLAD=1
    fi
fi

cmake --build "$build_dir"
