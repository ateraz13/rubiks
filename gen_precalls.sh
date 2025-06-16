#!/usr/bin/env bash

pre_callback_func=""
post_callback_func=""
input_file_count=0
input_files=()
output_header=""
output_source=""

allow_default=0
script_name="$0"

print_help() {
    cat <<EOF
$script_name [options...]
options:
    --files <input_file...>         : Input files used to to scrape debug calls.
    --output-src <output_source>    : Use <output_source> as the output C++ source file.
    --output-header <output_header> : Use <output_header> as the output C++ header file.
    --pre-cb <func_name>            : Use <func_name> as the name of the function called before every debug GL call.
    --post-cb <func_name>           : Use <func_name> as the nane of the function called after every debug GL call.
    --allow-default                 : Lets the program to use default options, for safety
                                      the defaults have to be explicitely enabled so files don't
                                      get overriden accidentally.
EOF
}

next_capture=""
for arg in "$@"; do
    if [[ "$next_capture" == "files" && "$arg" == "--*" ]]; then
        next_capture=""
    fi
    case "$next_capture" in
    "pre-cb")
        pre_callback_func="$arg"
        next_capture=""
        ;;
    "post-cb")
        post_callback_func="$arg"
        next_capture=""
        ;;
    "output-src")
        output_source="$arg"
        next_capture=""
        ;;
    "output-header")
        output_header="$arg"
        next_capture=""
        ;;
    "files")
        input_file_count=$(("$input_file_count" + 1))
        input_files+=("$arg")
        ;;
    *)
        case "$arg" in
        "--allow-default")
            allow_default=1
            ;;
        "--pre-cb")
            next_capture="pre-cb"
            ;;
        "--post-cb")
            next_capture="post-cb"
            ;;
        "--output-src" | "--out-src" | "--output-source" | "--cxx")
            next_capture="output-src"
            ;;
        "--output-header" | "--hxx")
            next_capture="output-header"
            ;;
        "--help")
            print_help
            exit
            ;;
        "--files")
            next_capture="files"
            ;;
        *)
            echo "Invalid argument: $arg"
            print_help
            ;;
        esac
        ;;
    esac
done

default_or_exit() {
    if [[ "$1" == "" ]]; then
        if [[ "$allow_default" -eq 1 ]]; then
            echo "$2"
        else
            echo "Error: Missing option and no default allowed!"
            print_help
            exit
        fi
    else
        echo "$1"
    fi
}

post_callback_func=$(default_or_exit "$post_callback_func" "postcall_callback")
pre_callback_func=$(default_or_exit "$pre_callback_func" "precall_callback")
output_source=$(default_or_exit "$output_source" gl_calls.cxx)
output_header=$(default_or_exit "$output_header" gl_calls.hxx)

for file in "${input_files[@]}" ; do
    if [[ ! -f $file ]] ; then
        echo "Error: File does not exit \"$file\". Aborting!"
        exit 1
    fi
done

echo "input_files = ${input_files[*]}"

if [[ "${#input_files[@]}" -eq 0 ]]; then
    echo "Error: No input files specified"
    print_help
    exit
fi
# echo "post_callback_func =  $post_callback_func"
# echo "pre_callback_func = $pre_callback_func"
# echo "output_source = $output_source"
# echo "output_header = $output_header"

# void re
{
    cat <<EOF
#ifndef GL_CALLS_HXX
#define GL_CALLS_HXX

#include <iostream>
#include <type_traits>
#define DBG_GL_PRE_CALLBACK $pre_callback_func
#define DBG_GL_POST_CALLBACK $post_callback_func
void $pre_callback_func(const char* source_file, int line_num, const char* gl_func);
void $post_callback_func(const char* source_file, int line_num, const char* gl_func);

void dbg_gl_print_args_internal ();
void dbg_gl_print_args ();

template<typename Arg, typename... Args>
void dbg_gl_print_args_internal (Arg arg, Args... rest) {
  std::cout << ", " << arg;
  dbg_gl_print_args_internal(rest...);
};

template<typename Arg, typename... Args>
void dbg_gl_print_args (Arg arg, Args... rest) {
   std::cout <<  arg;
   dbg_gl_print_args_internal(rest...);
};

// General GL_Call template
template <typename GL_Func, typename... Args>
static std::invoke_result_t<GL_Func, Args...> dbg_gl_call(GL_Func gl_func, const char* source_file, int line_num, const char* func_name, Args... args) {

   std::cout << source_file << ":" << line_num << ": " << func_name << "(";
   dbg_gl_print_args(args...);
   std::cout << ")\n";
   DBG_GL_PRE_CALLBACK(source_file, line_num, func_name);

   if constexpr (std::is_same_v<std::invoke_result_t<GL_Func, Args...>, void>) {
       gl_func(args...); // No return value
       DBG_GL_POST_CALLBACK(source_file, line_num, func_name);
   } else {
       auto ret = gl_func(args...); // Capture return value
       DBG_GL_POST_CALLBACK(source_file, line_num, func_name);
       std::cout << "gl_call_return: " << ret << "\n";
       return ret;
   }
}
#endif //GL_CALLS_HXX
EOF

} > "$output_header"

function add_to_header {
    echo "$@" >>"$output_header"
}

cat ${input_files[@]} | awk "match(\$0, /\s+inspect_([^(]+)\([^)]*\)/, names){ print names[1] }" | grep -v glfw | grep -v glew | sort | uniq |
    while read -r func_name; do
        {
            cat <<EOF
#ifdef INSPECT_GL_DEBUG_INFO
  #define inspect_$func_name(args...) \\
    dbg_gl_call($func_name, __FILE__, __LINE__, "$func_name", args)
#else
  #define inspect_$func_name(args...) \\
    $func_name(args)
#endif //INSPECT_GL_DEBUG_INFO

EOF
        } >>"$output_header"
    done
{
cat <<EOF
#include "$output_header"

void dbg_gl_print_args_internal () {}
void dbg_gl_print_args () {}
EOF
} > "$output_source"

