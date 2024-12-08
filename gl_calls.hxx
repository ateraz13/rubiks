#ifndef GL_CALLS_HXX
#define GL_CALLS_HXX

#include <iostream>
#include <type_traits>
#define DBG_GL_PRE_CALLBACK precall_callback
#define DBG_GL_POST_CALLBACK postcall_callback
void precall_callback(const char* source_file, int line_num, const char* gl_func);
void postcall_callback(const char* source_file, int line_num, const char* gl_func);

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

cat gfx.cxx game.cxx shader.cxx | awk "match($0, /\s+d(gl[^(]+)\([^)]*\)/, names){ print names[1] }"  | grep -v glfw | grep -v glew | sort | uniq |
while read -r func_name ; do
    echo "#define d(args...) \" >> "gl_calls.hxx"
    echo "  dbg_gl_call(, __FILE__, __LINE__, \"\", args)" >> "gl_calls.hxx"
done

cat <<EOF > "gl_calls.cxx"
#include "gl_calls.hxx"

void dbg_gl_print_args_internal () {}
void dbg_gl_print_args () {}
