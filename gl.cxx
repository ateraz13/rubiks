#include "gl.hxx"
#include <iostream>
#include <sstream>

void load_opengl_funcs(gl_loader_func proc) {
#ifdef USE_GLAD
    // gladLoadGLLoader((GLADloadfunc)glfwGetProcAddress);

    int version = gladLoadGL(proc);
    if (version == 0) {
        throw std::runtime_error("Failed to load OpenGL functions!");
    }

    std::cout << "Loaded Opengl " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;
#else

    auto err = glewInit();
    if(err != GLEW_OK) {
        std::stringstream ss;
        ss << "Failed to load OpenGL functions: ";
        ss << glewGetErrorString(err);
        throw std::runtime_error(ss.str());
    }

#endif
}
