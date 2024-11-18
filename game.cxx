#include "game.hxx"
#include "utility.hxx"
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>

#define DEBUG_MESSAGES

const int MAIN_WINDOW_DEFAULT_HEIGHT = 768;
const int MAIN_WINDOW_DEFAULT_WIDTH = 1024;
const char *MAIN_WINDOW_DEFAULT_TITLE = "Rubiks";

Game& Game::instance(){static Game game; return game;}

void glfw_error_callback(int error, const char *desc) {
  fprintf(stderr, "GLFW Error: %s\n", desc);
}

Game::Game()
    : m_rcube(), m_gfx(), m_animation_speed(1.0f), m_main_window(nullptr) {
  init_window_system();
  init_input_system();
  m_gfx.init();
}

Game::~Game() { save("before_exit_autosave"); }

void Game::handle_inputs(GLFWwindow *win, Game::KeyboardKey key, int scancode,
                         int action, int mods) {

  auto &game = Game::instance();

  if (win != game.m_main_window.get()) {
    throw std::runtime_error(
        "FIXME: Implement input for windows other than the main window!");
  }

#ifdef DEBUG_MESSAGES
  std::cout << "Event received: ";
  if (key == GLFW_KEY_ESCAPE) {
    std::cout << "ESCAPE\n";
  } else {
    std::cout << "UNKOWN KEY\n";
  }
  std::cout << std::endl;
#endif

  if (action == GLFW_PRESS || action == GLFW_RELEASE) {
    auto a = action == GLFW_PRESS ? Game::KeyState::PRESSED
                                  : Game::KeyState::RELEASED;
    Game::KeyEvent ke{game.m_main_window, key, a};

    if (auto a = game.m_keymap.find(ke); a != std::end(game.m_keymap)) {
#ifdef DEBUG_MESSAGES
      std::cout << "Adding action!\n";
      if (a->second == Game::Action::QUIT_GAME) {
        std::cout << "Adding quit game action!\n";
      }
#endif
      game.action_queue.push(a->second);
    }
  }
}

void Game::init_input_system() {

  // FIXME: Make sure the input_handler is never called by GLFW after Game has
  // been freed.

  glfwSetKeyCallback(m_main_window.get(), &handle_inputs);

  m_keymap[{m_main_window, GLFW_KEY_ESCAPE, KeyState::RELEASED}] =
      Game::Action::QUIT_GAME;

  for (auto &[ev, ac] : m_keymap) {
    std::cout << "MAP KEY: " << ev.key << " ACTION: " << ac << std::endl;
  }
}

void Game::init_window_system() {

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    throw std::runtime_error("Could not initialise GLFW!");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  {
    GLFWwindow *mw =
        glfwCreateWindow(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT,
                         MAIN_WINDOW_DEFAULT_TITLE, NULL, NULL);

    if (mw == nullptr) {
      glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
      glfwCreateWindow(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT,
                       MAIN_WINDOW_DEFAULT_TITLE, NULL, NULL);
      if (mw == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window!");
      }
    }

    m_main_window = std::shared_ptr<GLFWwindow>(
        mw, [=](GLFWwindow *win) { glfwDestroyWindow(win); });

    glfwMakeContextCurrent(m_main_window.get());
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
  }
}

void Game::save(const std::string &name) {
  // TODO: Save the game
}

void Game::update_current_time() {
  m_last_time = m_current_time;
  m_current_time = glfwGetTime(); // FIXME: Update current time!
  uint64_t delta_time = m_current_time - m_last_time;
}

void Game::start() {

  if (m_is_running) {
    throw std::runtime_error("Game is already running!");
  }

  m_is_running = true;
  while (m_is_running) {
    update_current_time();
    update();
  }
}

void Game::stop() { m_is_running = false; }

void Game::update() {

  while (action_queue.size() > 0) {
    auto action = action_queue.front();

#ifdef DEBUG_MESSAGES
    std::cout << "Found action #" << action << "!\n";
#endif

    switch (action) {
    case QUIT_GAME:
      std::cout << "Quiting game!\n";
      this->stop();
      break;
    case ROTATE_1ST_COLUMN_FORWARD:
      m_rcube.rotate_1st_column_forward();
      break;
    case ROTATE_2ND_COLUMN_FORWARD:
      m_rcube.rotate_2nd_column_forward();
      break;
    case ROTATE_3RD_COLUMN_FORWARD:
      m_rcube.rotate_3rd_column_forward();
      break;
    case ROTATE_1ST_COLUMN_BACKWARDS:
      m_rcube.rotate_1st_column_backwards();
      break;
    case ROTATE_2ND_COLUMN_BACKWARDS:
      m_rcube.rotate_2nd_column_backwards();
      break;
    case ROTATE_3RD_COLUMN_BACKWARDS:
      m_rcube.rotate_3rd_column_backwards();
      break;
    case ROTATE_1ST_ROW_FORWARD:
      m_rcube.rotate_1st_row_forward();
      break;
    case ROTATE_2ND_ROW_FORWARD:
      m_rcube.rotate_2nd_row_forward();
      break;
    case ROTATE_3RD_ROW_FORWARD:
      m_rcube.rotate_3rd_row_forward();
      break;
    case ROTATE_1ST_ROW_BACKWARDS:
      m_rcube.rotate_1st_row_backwards();
      break;
    case ROTATE_2ND_ROW_BACKWARDS:
      m_rcube.rotate_2nd_row_backwards();
      break;
    case ROTATE_3RD_ROW_BACKWARDS:
      m_rcube.rotate_3rd_row_backwards();
      break;
    default:
      break;
    }

    action_queue.pop();
  }

  glClearColor(0.09f, 0.0, 0.12f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_gfx.draw();

  glfwSwapBuffers(m_main_window.get());
  glfwPollEvents();
}

bool Game::KeyEvent::operator<(const Game::KeyEvent &other) const {
  if (auto win1 = window.lock(), win2 = other.window.lock(); win1 < win2) {
    return -1;
  } else if (win1 == win2) {
    if (key < other.key) {
      return -1;
    } else if (key == other.key) {
      if (state < other.state) {
        return -1;
      } else if (state == other.state) {
        return 0;
      } else {
        return 1;
      }
    } else {
      return 1;
    }
  } else {
    return 1;
  }
}

void RubiksCube::rotate_1st_column_forward() {
  std::cout << "rotate_1st_column_forward" << std::endl;
}

void RubiksCube::rotate_2nd_column_forward() {
  std::cout << "rotate_2nd_column_forward" << std::endl;
}

void RubiksCube::rotate_3rd_column_forward() {
  std::cout << "rotate_3rd_column_forward" << std::endl;
}

void RubiksCube::rotate_1st_column_backwards() {
  std::cout << "rotate_1st_column_backwards" << std::endl;
}

void RubiksCube::rotate_2nd_column_backwards() {
  std::cout << "rotate_2nd_column_backwards" << std::endl;
}

void RubiksCube::rotate_3rd_column_backwards() {
  std::cout << "rotate_3rd_column_backwards" << std::endl;
}

void RubiksCube::rotate_1st_row_forward() {
  std::cout << "rotate_1st_row_forward" << std::endl;
}

void RubiksCube::rotate_2nd_row_forward() {
  std::cout << "rotate_2nd_row_forward" << std::endl;
}

void RubiksCube::rotate_3rd_row_forward() {
  std::cout << "rotate_3rd_row_forward" << std::endl;
}

void RubiksCube::rotate_1st_row_backwards() {
  std::cout << "rotate_1st_row_backwards" << std::endl;
}

void RubiksCube::rotate_2nd_row_backwards() {
  std::cout << "rotate_2nd_row_backwards" << std::endl;
}

void RubiksCube::rotate_3rd_row_backwards() {
  std::cout << "rotate_3rd_row_backwards" << std::endl;
}

void RubiksCube::reset() { std::cout << "reset" << std::endl; }
