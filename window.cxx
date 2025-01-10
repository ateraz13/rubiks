#include "window.hxx"
#include "except.hxx"
#include <GLFW/glfw3.h>
#include "game.hxx"
#include <optional>

SystemWindow::SystemWindow() : m_win_handle(nullptr), m_ref_count(new int(0)) {}

static bool is_glfw_initialized = false;
static size_t system_window_count = 0;

void SystemWindow::init(const SystemWindowConfig &config) {
  if (!is_glfw_initialized && !glfwInit()) {
    throw WindowingLibraryInitFailed("Failed to initialize glfw!\n");
  }

  is_glfw_initialized = true;

  if (config.opengl_version) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.opengl_version->major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.opengl_version->minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }

  auto win = glfwCreateWindow(config.width, config.height, config.title.c_str(),
                              NULL, NULL);

  if (win == nullptr) {
    throw SystemWindowFailedToCreate("Could not create GLFW window!\n");
  }

  glfwSetKeyCallback(win, &WindowSystem::redirect_inputs);
  system_window_count += 1;
}

SystemWindow::~SystemWindow() {
  if (m_initialized) {
    std::cout << "Destroying window instance!\n";
    *m_ref_count -= 1;
    if(*m_ref_count < 0) {
      std::runtime_error("Fatal error: ref-count below zero on SystemWindow object!");
    }
    if(m_ref_count == 0) {
      std::cout << "Destroying window!\n";
      system_window_count -= 1;
      is_glfw_initialized = false;
      glfwDestroyWindow(m_win_handle);
      delete m_ref_count;
      glfwTerminate();
    }
  }
}

void SystemWindow::swap_buffers() { glfwSwapBuffers(m_win_handle); }

void SystemWindow::swap(SystemWindow &a, SystemWindow &b) {
  std::swap(a.m_initialized, b.m_initialized);
  std::swap(a.m_ref_count, b.m_ref_count);
  std::swap(a.m_win_handle, b.m_win_handle);
}

SystemWindow::SystemWindow(const SystemWindow &other) {
  m_win_handle = other.m_win_handle;
  m_ref_count = other.m_ref_count;
  *m_ref_count += 1;
}

SystemWindow &SystemWindow::operator=(const SystemWindow &other) {
  auto tmp = SystemWindow(other);
  std::swap(tmp, *this);
  return *this;
}

void SystemWindow::bind_context() { glfwMakeContextCurrent(m_win_handle); }

SystemWindowBuilder WindowSystem::make(std::string window_purpose) {
  return SystemWindowBuilder(std::move(window_purpose));
}

SystemWindowBuilder::SystemWindowBuilder(std::string purpose) {
  m_config.purpose = std::move(purpose);
}

SystemWindowConfig::SystemWindowConfig() {}

SystemWindowBuilder &SystemWindowBuilder::with_size(int w, int h) {
  m_config.width = w;
  m_config.height = h;
  return *this;
}

SystemWindowBuilder &SystemWindowBuilder::with_opengl(int major_version,
                                                      int minor_version) {
  m_config.opengl_version = SystemWindowConfig::GLVersion{
      .major = major_version, .minor = minor_version};

  return *this;
}

SystemWindowBuilder &SystemWindowBuilder::with_title(const std::string &title) {
  m_config.title = title;
  return *this;
}

SystemWindow SystemWindowBuilder::build() {
  auto win = SystemWindow();
  win.init(m_config);
  return win;
}

WindowSystem::WindowSystem() {}

WindowSystem &WindowSystem::instance() {
  static WindowSystem inst;
  return inst;
}

void WindowSystem::redirect_inputs(GLFWwindow *handle, KeyboardKey key,
                                   int scancode, int key_state_native,
                                   int mods) {

//   auto &game = Game::instance();
//   auto &ws = WindowSystem::instance();
//   auto win = ws.find_system_window(handle);

//   if (!win) {
//     throw EventOnUnregisteredWindow("Event received on unregistered window."
//                                     "The window must have been created without "
//                                     "the acknowledment of WindowSystem.");
//   }

// #ifdef DEBUG_MESSAGES
//   std::cout << "Event received: ";
//   if (key == GLFW_KEY_ESCAPE) {
//     std::cout << "ESCAPE\n";
//   } else {
//     std::cout << "UNKOWN KEY\n";
//   }
//   std::cout << std::endl;
// #endif

//   if (key_state_native == GLFW_PRESS || key_state_native == GLFW_RELEASE) {
//     auto key_state =
//         key_state_native == GLFW_PRESS ? KeyState::PRESSED : KeyState::RELEASED;
//     KeyEvent ke{*win, key, key_state};

//     if (auto key_bind = game.m_keymap.find(ke);
//         key_bind != std::end(game.m_keymap)) {
// #ifdef DEBUG_MESSAGES
//       std::cout << "Running action!\n";
// #endif
//       // Run the action.
//       (*(key_bind->second))();
//     }
//   }
// }

// std::optional<SystemWindow> WindowSystem::find_system_window(SystemWindowHandle handle) {
//   for(auto w: m_system_windows) {
//     if(w.second.m_win_handle == handle) {
//       return w.second;
//     }
//   }

//   return {};
}

void WindowSystem::poll_events() {
  glfwPollEvents();
}

bool SystemWindow::operator<(const SystemWindow &other) const {
  return m_win_handle < other.m_win_handle;
}

bool SystemWindow::operator==(const SystemWindow &other) const {
  return m_win_handle == other.m_win_handle;
}

bool SystemWindow::operator>(const SystemWindow &other) const {
  return m_win_handle > other.m_win_handle;
}

bool KeyEvent::operator<(const KeyEvent &other) const {
  return window < other.window && key < other.key && state < other.state;
}

bool KeyEvent::operator==(const KeyEvent &other) const {
  return window == other.window && key == other.key && state == other.state;
}

bool KeyEvent::operator>(const KeyEvent &other) const {
  return window > other.window && key > other.key && state > other.state;
}
