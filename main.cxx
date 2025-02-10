#include <iostream>
#include "gfx.hxx"
#include "game.hxx"
#include "utility.hxx"

int main(int argc, char** argv) {
    auto& game = Game::instance();

#ifdef INSPECT_GL_DEBUG_INFO
    std::cout << "OpenGL call inspecting is enabled!\n";
#endif

    game.start();

    std::cout << "Hello, world!\n";

    return 0;
}
