#!/usr/bin/env bash

if [ $# -lt 3 ]; then
    echo "$0 <input_files...> <pre_callback_func> <post_callback_func>"
    exit 1
fi

cmd_args=( "$@" )
pre_callback_func=${cmd_args[$(($# - 2))]}
post_callback_func=${cmd_args[$(($# - 1))]}
input_file_count=$(($#-2))
input_files=${@:1:$input_file_count}

echo "#ifndef GL_CALLS_HXX"
echo "#define GL_CALLS_HXX"
echo "#include <iostream>"
echo "#include <type_traits>"
echo "#define DBG_GL_PRE_CALLBACK $pre_callback_func"
echo "#define DBG_GL_POST_CALLBACK $post_callback_func"
echo "void $pre_callback_func(const char* source_file, int line_num, const char* gl_func);"
echo "void $post_callback_func(const char* source_file, int line_num, const char* gl_func);"
echo " "
echo "void dbg_gl_print_args_internal() {}"
echo " "
echo "template<typename Arg, typename... Rest>"
echo "void dbg_gl_print_args_internal(Arg arg, Rest... rest) {"
echo "   std::cout << \", \" << arg;"
echo "   dbg_gl_print_args_internal(rest...);"
echo "}"
echo " "
echo "void dbg_gl_print_args() {}"
echo " "
echo "template<typename Arg, typename... Rest>"
echo "void dbg_gl_print_args(Arg arg, Rest... rest) {"
echo "   std::cout <<  arg;"
echo "   dbg_gl_print_args_internal(rest...);"
echo "}"
echo " "
# void return value calls
cat <<EOM
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
EOM
cat ${input_files} | awk "match(\$0, /\s+d(gl[^(]+)\([^)]*\)/, names){ print names[1] }"  | grep -v glfw | grep -v glew | sort | uniq |
while read -r func_name ; do
    echo "#define d$func_name(args...) \\"
    echo "  dbg_gl_call($func_name, __FILE__, __LINE__, \"$func_name\", args)"
done
echo "#endif //GL_CALLS_HXX"
