#!/usr/bin/env bash

build_dir=./build-x86-64-linux
do_clean_build=0
do_debug_build=0
do_gl_debug_build=0
do_build=0
do_run=0
toolchain=""
script_name="$0"
debug_flags=()
cxx_flags=("-DCMAKE_EXPORT_COMPILE_COMMANDS=1")
produce_lsp_extras=0

function print_help {
    cat <<EOF
$script_name [options...]
options:
    --help                  : Print this message.
    --build-dir <build_dir> : Use <build_dir> as the build directory.
    --build                 : Build the project.
    --debug                 : Build with debug systems
    --clean-build           : Do clean build of the project(Rebuild).
    --run                   : Run the project.
                              If the build directory is absent it also builds
                              the project beforehand.
    --lsp                   : Setup the project directory to be used with an LSP.
                              e.g: compile_commands.json.
    --build-and-run         : Builds and runs the project.
EOF
}

next_capture_value=""
for x in "$@"; do
    case "$next_capture_value" in
    "build_dir")
        build_dir="$x"
        next_capture_value=""
        ;;
    "toolchain")
        toolchain="$x"
        next_capture_value=""
        ;;
    *)
        next_capture_value=""
        case "$x" in
        "--help")
            print_help
            exit 0
            ;;
        "--debug")
            do_debug_build=1
            debug_flags+=("-DCMAKE_BUILD_TYPE=Debug")
            ;;
        "--clean-build")
            do_clean_build=1
            do_build=1
            ;;
        "--build")
            do_build=1
            ;;
        "--run")
            do_run=1
            ;;
        "--build-and-run")
            do_build=1
            do_run=1
            ;;
        "--build-dir")
            next_capture_value="build_dir"
            ;;
        "--toolchain")
            next_capture_value="toolchain"
            ;;
        "--gl-debug")
            do_gl_debug_build=1
            debug_flags+=("-DPROVIDE_INSPECT_GL_DEBUG_INFO=1")
            ;;
        "--lsp")
            produce_lsp_extras=1
            ;;
        *)
            echo "Invalid option: $x"
            print_help
            exit 1
            ;;
        esac
        ;;
    esac
done

build_dir=$(cd "$build_dir" || exit ; pwd)

if [[ "$do_gl_debug_build" -eq 1 ]] ; then
    rm gl_calls.cxx gl_calls.hxx
    mapfile -t source_files < <(find . -name "*.cxx")
    bash gen_precalls.sh --pre-cb precall_callback --post-cb postcall_callback --hxx gl_calls.hxx --cxx gl_calls.cxx --files ${source_files[@]}
fi

if [[ ! -d "$build_dir" && "$do_run" -eq 1 && ! "$do_build" -eq 1 ]] ; then
    do_build=1
fi

if [[ "$do_build" -eq 1 && ! "$do_clean_build" -eq 1 && ! -d "$build_dir" ]] ; then
    do_clean_build=1
fi

if [[ -d "$build_dir" && "$do_clean_build" -eq 1 ]]; then
    rm -R "$build_dir"
fi

if [[ ! -d "$build_dir" && "$do_clean_build" -eq 1 ]]; then
    rm -R "$build_dir"
    if [ "$toolchain" != "" ]; then
        cmake -B"$build_dir" -DCMAKE_TOOLCHAIN_FILE="$PWD/$toolchain.cmake" -DUSE_GLAD=1 ${debug_flags[@]} ${cxx_flags[@]}
    else
        cmake -B"$build_dir" -DUSE_GLAD=1 ${debug_flags[@]} ${cxx_flags[@]}
    fi
fi

if [[ -d "$build_dir" && "$produce_lsp_extras" -eq 1 ]] ; then
    if [[ -L "$PWD/compile_commands.json" ]] ; then
        unlink "$PWD/compile_commands.json"
    fi
    ln -s "$build_dir/compile_commands.json" "$PWD/compile_commands.json"
fi

if [ "$do_build" -eq 1 ]; then
    cmake --build "$build_dir" -- -j "$(nproc)"
fi

if [[ ! -d "$build_dir/shaders" ]] ; then
    if [[ -L "$build_dir/shaders" &&  $(readlink -f "$build_dir/shaders") == "$PWD/shaders" ]] ; then
        unlink "$build_dir/shaders"
        ln -s "$PWD/shaders" "$build_dir"
    fi
fi

if [ "$do_run" -eq 1 ]; then
    "$build_dir/rubiks"
fi

