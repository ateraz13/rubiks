#include "game.hxx"
#include "except.hxx"
#include "gl.hxx"
#include "gl_calls.hxx"
#include "utility.hxx"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

#define DEBUG_MESSAGES

const int MAIN_WINDOW_DEFAULT_HEIGHT = 768;
const int MAIN_WINDOW_DEFAULT_WIDTH = 1024;
const char *MAIN_WINDOW_DEFAULT_TITLE = "Rubiks";

Game &Game::instance() {
  static auto &game = Game::init();
  return game;
}

void glfw_error_callback(int error, const char *desc) {
  fprintf(stderr, "GLFW Error: %s\n", desc);
}

Game::Game() {}

Game &Game::init() {
  static Game game;
  std::cout << "Initialising game!\n";
  game.init_window_system();
  game.init_input_system();
  game.m_gfx.init();

  return game;
}

Game::~Game() {
  std::clog << "Destroying game!\n";
  // save("before_exit_autosave");
}

void Game::handle_inputs(GLFWwindow *win, Game::KeyboardKey key, int scancode,
                         int action, int mods) {

  auto &game = Game::instance();

  if (win != game.m_main_window.get()) {
    throw NotImplemented(
        "Support for multiple windows is not implemted\n"
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
      game.m_action_queue.push(a->second);
    }
  }
}

void main_window_resized_cb(GLFWwindow *window, int width, int height) {
  auto &inst = Game::instance();
  inst.acknowledge_main_window_resize(width, height);
}

void Game::acknowledge_main_window_resize(int width, int height) {
  std::cout << "Window resized: width = " << width << ", height = " << height
            << "\n";
  glfwMakeContextCurrent(m_main_window.get());
  m_gfx.viewport_size(width, height);
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

  if (!glfwInit()) {
    throw std::runtime_error("Could not initialise GLFW!");
  }

  glfwSetErrorCallback(glfw_error_callback);

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

    glfwSetWindowSizeCallback(mw, main_window_resized_cb);
    glfwMakeContextCurrent(mw);

    m_main_window = std::shared_ptr<GLFWwindow>(mw, [=](GLFWwindow *win) {
      std::cout << "Destoying window!\n";
      glfwDestroyWindow(win);
      glfwTerminate();
    });

    EXPR_LOG(m_main_window.get());

    load_opengl_funcs(&glfwGetProcAddress);

    glfwSwapInterval(0);
    m_gfx.viewport_size(MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT);
  }
}

void Game::save(const std::string &name) {
  // TODO: Save the game
  throw NotImplemented("Save functionality is not implemented!\n");
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

  using namespace std::chrono;

  while (m_action_queue.size() > 0) {
    auto action = m_action_queue.front();

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

    m_action_queue.pop();
  }

  auto frame_begin_time = steady_clock::now();

  glfwMakeContextCurrent(m_main_window.get());
  glClearColor(0.12f, 0.0, 0.12f, 1.0f);
  // glClearDepth(10.0f);
  glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_ALWAYS);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);

  EXPR_LOG(m_main_window.get());

  m_gfx.draw();

  glfwSwapBuffers(m_main_window.get());
  glfwPollEvents();

  using namespace std::chrono_literals;
  using std::chrono::duration;

  auto framerate = 2;

  auto elapsed_duration = duration_cast<milliseconds>(
      milliseconds(1000/framerate) - (frame_begin_time - m_last_frame_timepoint));

  auto sleep_duration =
      std::max(elapsed_duration, 0ms);
  std::this_thread::sleep_for(sleep_duration);
  m_last_frame_timepoint = frame_begin_time;
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

double Game::current_time() const {
  return m_current_time;
}
