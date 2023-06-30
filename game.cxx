#include "game.hxx"

CellID RubiksCube::ray_intersection() const { return -1; }

Game::Game(GameSettings settings) : m_rcube(), m_gfx() {
  m_gfx.init(settings.get_graphical_settings());
}

Game::~Game() { save("before_exit_autosave"); }

void Game::get_current_time() const {
    m_last_time = m_current_time;
    m_current_time = 0; // FIXME: Update current time!
    uint64_t delta_time = m_current_time - m_last_time;
}

void Game::start() {
    m_is_running = true;
    while (m_is_running) {
        update_current_time();
        update();
    }
}

void Game::stop() {
    m_is_running = false;
}

void Game::update() {
  if (m_gfx.has_settings_changed()) {
    // Do something
  }
}
