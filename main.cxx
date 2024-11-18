#include <iostream>
#include "gfx.hxx"
#include "game.hxx"
#include "utility.hxx"

int main(int argc, char** argv) {



    auto& game = Game::instance();

    game.start();

    std::cout << "Hello, world!\n";

    return 0;
}
