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
echo "   print_args_internal(rest...);"
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
echo "template<typename GL_Func, typename ReturnType = void>"
echo "struct GL_Call {"
echo "};"
echo " "
# void return value calls
echo "template<typename GL_Func>"
echo "struct GL_Call<GL_Func, std::enable_if_t<std::is_same_v<std::invoke_result_t<GL_Func>, void>>> {"
echo " template<typename... Args> "
echo " static void execute(GL_Func gl_func, const char* source_file, int line_num, const char* func_name, Args... args) { "
echo "    std::cout << \"gl_call: \" << func_name << \"(\";"
echo "    dbg_gl_print_args(args...);"
echo "    std::cout << \")\";"
echo "    DBG_GL_PRE_CALLBACK(source_file, line_num, func_name);"
echo "    gl_func(args...);"
echo "    DBG_GL_POST_CALLBACK(source_file, line_num, func_name);"
echo " }"
echo "};"
echo " "
# Non void return value calls
echo "template<typename GL_Func>"
echo "struct GL_Call<GL_Func, std::enable_if_t<!std::is_same_v<std::invoke_result_t<GL_Func>, void>>> {"
echo " template<typename... Args> "
echo " static void execute(GL_Func gl_func, const char* source_file, int line_num, const char* func_name, Args... args) { "
echo "    std::cout << \"gl_call: \" << func_name << \"(\";"
echo "    dbg_gl_print_args(args...);"
echo "    std::cout << \")\";"
echo "    DBG_GL_PRE_CALLBACK(source_file, line_num, func_name);"
echo "    auto ret = gl_func(args...);"
echo "    std::cout << \"gl_call_return: \" << ret << \"\n\" ;"
echo "    DBG_GL_POST_CALLBACK(source_file, line_num, func_name);"
echo "    return ret;"
echo " }"
echo "};"
echo " "
awk "match(\$0, /\s+(gl[^(]+)\([^)]*\)/, names){ print names[1] }" "$input_files" | grep -v glfw | grep -v glew | sort | uniq |
while read -r func_name ; do
    echo "#define d$func_name(args...) \\"
    echo "  GL_Call<decltype($func_name)>::execute($func_name, __FILE__, __LINE__, \"$func_name\", args)"
done
echo "#endif //GL_CALLS_HXX"
