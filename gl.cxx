#include "gl.hxx"

void load_opengl_funcs(void* proc) {
#ifdef USE_GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int version = gladLoadGL(proc);
    if (version == 0) {
        throw std::runtime_error("Failed to load OpenGL functions!");
    }

    std::cout << "Loaded Opengl " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;
#else

    if(glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to load OpenGL functions!");
    }

#endif
}
