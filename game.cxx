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

void Game::stop() { m_is_running = false; }

void Game::update() {
  if (m_gfx.has_settings_changed()) {
    // Do something
  }

  auto input_key = 0; // FIXME: Actually Use value from input event.

  if (auto v = m_keymap.find(input_key); v != m_keymap.end()) {

    switch (v->second) {
    ROTATE_1ST_COLUMN_FORWARD:
      m_rcube.rotate_1st_column_forward();
      break;
    ROTATE_2ND_COLUMN_FORWARD:
      m_rcube.rotate_2nd_column_forward();
      break;
    ROTATE_3RD_COLUMN_FORWARD:
      m_rcube.rotate_3rd_column_forward();
      break;
    ROTATE_1ST_COLUMN_BACKWARDS:
      m_rcube.rotate_1st_column_backwards();
      break;
    ROTATE_2ND_COLUMN_BACKWARDS:
      m_rcube.rotate_2nd_column_backwards();
      break;
    ROTATE_3RD_COLUMN_BACKWARDS:
      m_rcube.rotate_3rd_column_backwards();
      break;
    ROTATE_1ST_ROW_FORWARD:
      m_rcube.rotate_1st_row_forward();
      break;
    ROTATE_2ND_ROW_FORWARD:
      m_rcube.rotate_2nd_row_forward();
      break;
    ROTATE_3RD_ROW_FORWARD:
      m_rcube.rotate_3rd_row_forward();
      break;
    ROTATE_1ST_ROW_BACKWARDS:
      m_rcube.rotate_1st_row_backwards();
      break;
    ROTATE_2ND_ROW_BACKWARDS:
      m_rcube.rotate_2nd_row_backwards();
      break;
    ROTATE_3RD_ROW_BACKWARDS:
      m_rcube.rotate_3rd_row_backwards();
      break;
    default:
      break;
    }
  }
}
