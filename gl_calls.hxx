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
#define dglBindBuffer(args...) \
  dbg_gl_call(glBindBuffer, __FILE__, __LINE__, "glBindBuffer", args)
#define dglBindVertexArray(args...) \
  dbg_gl_call(glBindVertexArray, __FILE__, __LINE__, "glBindVertexArray", args)
#define dglBufferData(args...) \
  dbg_gl_call(glBufferData, __FILE__, __LINE__, "glBufferData", args)
#define dglDeleteBuffers(args...) \
  dbg_gl_call(glDeleteBuffers, __FILE__, __LINE__, "glDeleteBuffers", args)
#define dglDeleteVertexArrays(args...) \
  dbg_gl_call(glDeleteVertexArrays, __FILE__, __LINE__, "glDeleteVertexArrays", args)
#define dglDrawElements(args...) \
  dbg_gl_call(glDrawElements, __FILE__, __LINE__, "glDrawElements", args)
#define dglEnableVertexAttribArray(args...) \
  dbg_gl_call(glEnableVertexAttribArray, __FILE__, __LINE__, "glEnableVertexAttribArray", args)
#define dglGenBuffers(args...) \
  dbg_gl_call(glGenBuffers, __FILE__, __LINE__, "glGenBuffers", args)
#define dglGenVertexArrays(args...) \
  dbg_gl_call(glGenVertexArrays, __FILE__, __LINE__, "glGenVertexArrays", args)
#define dglUseProgram(args...) \
  dbg_gl_call(glUseProgram, __FILE__, __LINE__, "glUseProgram", args)
#define dglVertexAttribPointer(args...) \
  dbg_gl_call(glVertexAttribPointer, __FILE__, __LINE__, "glVertexAttribPointer", args)
#define dglViewport(args...) \
  dbg_gl_call(glViewport, __FILE__, __LINE__, "glViewport", args)
