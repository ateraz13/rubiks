#include "app.hxx"

App::App() {}

App& App::instance() {
    static App app;
    return app;
}

App::~App() {}

void App::init() {
    m_game.init();
    m_game.start();
}

Game &App::game() { return m_game; }

const Game &App::game() const { return m_game; }

WindowSystem &App::win_sys() { return m_win_sys; }

const WindowSystem &App::win_sys() const { return m_win_sys; }
