#include <iostream>
#include <rubiks/gfx.hxx>
#include <rubiks/game.hxx>
#include <rubiks/utility.hxx>
#include <rubiks/app.hxx>

int main(int argc, char** argv) {
    App &app = App::instance();

#ifdef INSPECT_GL_DEBUG_INFO
    std::cout << "OpenGL call inspecting is enabled!\n";
#endif

    app.init();

    std::cout << "Hello, world!\n";

    return 0;
}
