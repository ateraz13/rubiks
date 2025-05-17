#include <iostream>
#include "gfx.hxx"
#include "game.hxx"
#include "utility.hxx"
#include "app.hxx"

int main(int argc, char** argv) {
    App &app = App::instance();

#ifdef INSPECT_GL_DEBUG_INFO
    std::cout << "OpenGL call inspecting is enabled!\n";
#endif

    app.init();

    std::cout << "Hello, world!\n";

    return 0;
}
