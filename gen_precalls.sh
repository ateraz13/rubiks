#!/usr/bin/env bash

if [ $# -lt 5 ]; then
    echo "$0 <input_files...> <pre_callback_func> <post_callback_func> <output_header_file> <output_source_file>"
    exit 1
fi

cmd_args=( "$@" )
pre_callback_func=${cmd_args[$(($# - 4))]}
post_callback_func=${cmd_args[$(($# - 3))]}
input_file_count=$(($#-4))
input_files=${@:1:$input_file_count}
output_header=${cmd_args[$(($# - 2))]}
output_source=${cmd_args[$(($# - 1))]}

# void return value calls
cat <<EOF > "$output_header"
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
EOM

cat ${input_files} | awk "match(\$0, /\s+d(gl[^(]+)\([^)]*\)/, names){ print names[1] }"  | grep -v glfw | grep -v glew | sort | uniq |
while read -r func_name ; do
    echo "#define d$func_name(args...) \\" >> "$output_header"
    echo "  dbg_gl_call($func_name, __FILE__, __LINE__, \"$func_name\", args)" >> "$output_header"
done

cat <<EOF > "$output_source"
#include "$output_header"

void dbg_gl_print_args_internal () {}
void dbg_gl_print_args () {}
EOF
