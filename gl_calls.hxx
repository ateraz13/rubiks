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
#ifdef ULTRA_GL_DEBUG_INFO
#define dglBindBuffer(args...) \
  dbg_gl_call(glBindBuffer, __FILE__, __LINE__, "glBindBuffer", args)
#else 
#define dglBindBuffer(args...) \
  glBindBuffer(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglBindVertexArray(args...) \
  dbg_gl_call(glBindVertexArray, __FILE__, __LINE__, "glBindVertexArray", args)
#else 
#define dglBindVertexArray(args...) \
  glBindVertexArray(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglBufferData(args...) \
  dbg_gl_call(glBufferData, __FILE__, __LINE__, "glBufferData", args)
#else 
#define dglBufferData(args...) \
  glBufferData(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglDeleteBuffers(args...) \
  dbg_gl_call(glDeleteBuffers, __FILE__, __LINE__, "glDeleteBuffers", args)
#else 
#define dglDeleteBuffers(args...) \
  glDeleteBuffers(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglDeleteVertexArrays(args...) \
  dbg_gl_call(glDeleteVertexArrays, __FILE__, __LINE__, "glDeleteVertexArrays", args)
#else 
#define dglDeleteVertexArrays(args...) \
  glDeleteVertexArrays(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglDrawArrays(args...) \
  dbg_gl_call(glDrawArrays, __FILE__, __LINE__, "glDrawArrays", args)
#else 
#define dglDrawArrays(args...) \
  glDrawArrays(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglEnableVertexAttribArray(args...) \
  dbg_gl_call(glEnableVertexAttribArray, __FILE__, __LINE__, "glEnableVertexAttribArray", args)
#else 
#define dglEnableVertexAttribArray(args...) \
  glEnableVertexAttribArray(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglGenBuffers(args...) \
  dbg_gl_call(glGenBuffers, __FILE__, __LINE__, "glGenBuffers", args)
#else 
#define dglGenBuffers(args...) \
  glGenBuffers(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglGenVertexArrays(args...) \
  dbg_gl_call(glGenVertexArrays, __FILE__, __LINE__, "glGenVertexArrays", args)
#else 
#define dglGenVertexArrays(args...) \
  glGenVertexArrays(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglUseProgram(args...) \
  dbg_gl_call(glUseProgram, __FILE__, __LINE__, "glUseProgram", args)
#else 
#define dglUseProgram(args...) \
  glUseProgram(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglVertexAttribPointer(args...) \
  dbg_gl_call(glVertexAttribPointer, __FILE__, __LINE__, "glVertexAttribPointer", args)
#else 
#define dglVertexAttribPointer(args...) \
  glVertexAttribPointer(args)
#endif //ULTRA_GL_DEBUG_INFO
#ifdef ULTRA_GL_DEBUG_INFO
#define dglViewport(args...) \
  dbg_gl_call(glViewport, __FILE__, __LINE__, "glViewport", args)
#else 
#define dglViewport(args...) \
  glViewport(args)
#endif //ULTRA_GL_DEBUG_INFO
