#ifndef GL_HXX
#define GL_HXX

#ifdef USE_GLAD
#include <glad/gl.h>
#else
#include <GL/glew.h>
#endif

#endif // GL_HXX

using gl_func = void (*) (void);
using gl_loader_func = gl_func (*)(const char* name);

void load_opengl_funcs(gl_loader_func proc);
