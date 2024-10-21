#include "game.hxx"
#include <stdexcept>
#include "utility.hxx"
#include <cstring>

const int MAIN_WINDOW_DEFAULT_HEIGHT = 768;
const int MAIN_WINDOW_DEFAULT_WIDTH = 1034;
const char *MAIN_WINDOW_DEFAULT_TITLE = "Rubiks";

void glfw_error_callback(int error, const char* desc) {
  fprintf(stderr, "GLFW Error: %s\n", desc);
}

Game::Game()
    : m_rcube(), m_gfx(), m_animation_speed(1.0f), main_window(nullptr) {
  init_window_system();
  m_gfx.init();
}

Game::~Game() { save("before_exit_autosave"); }

void Game::init_input_system() {

}

void Game::init_window_system() {

  glfwSetErrorCallback(glfw_error_callback);
  {
    GLFWwindow *mw =
        glfwCreateWindow(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT,
                         MAIN_WINDOW_DEFAULT_TITLE, NULL, NULL);

    if(mw == nullptr) {
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
      glfwCreateWindow(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT,
                       MAIN_WINDOW_DEFAULT_TITLE, NULL, NULL);
      if(mw == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window!");
      }
    }

    main_window = std::shared_ptr<GLFWwindow>(
        mw, [=](GLFWwindow *win) { glfwDestroyWindow(win); });
  }
}

void Game::save(const std::string &name) {
  // TODO: Save the game
}

void Game::update_current_time() {
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
