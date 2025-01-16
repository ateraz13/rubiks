#include "window.hxx"
#include "except.hxx"
#include "game.hxx"
#include <GLFW/glfw3.h>
#include <optional>

static std::atomic<bool> is_glfw_initialized = false;
static std::atomic<size_t> system_window_count = 0;

SystemWindow::SystemWindow() : m_win_handle(nullptr), m_ref_count(new int(1)) {}

SystemWindow::SystemWindow(SystemWindow &&other)
    : m_win_handle(other.m_win_handle), m_ref_count(other.m_ref_count),
      m_initialized(other.m_initialized) {
  std::cout << "Move construting system window!\n" << std::endl;

  clean_up();

  m_win_handle = other.m_win_handle;
  m_initialized = other.m_initialized;
  m_ref_count = other.m_ref_count;

  other.m_win_handle = nullptr;
  other.m_initialized = false;
  other.m_ref_count = nullptr;
}

SystemWindow::~SystemWindow() { clean_up(); }

void SystemWindow::clean_up() {
  if (m_initialized) {
    std::cout << "Destroying window instance!\n";
    *m_ref_count -= 1;
    if (*m_ref_count < 0) {
      std::cout << "SystemWindow: ref count below zero!\n";
    }
    if (m_ref_count == 0) {
      std::cout << "Destroying window!\n";
      system_window_count -= 1;
      is_glfw_initialized = false;
      glfwDestroyWindow(m_win_handle);
      delete m_ref_count;
    }
  }
}

SystemWindow &SystemWindow::operator=(SystemWindow &&other) {
  std::cout << "Move assigning system window!\n" << std::endl;

  clean_up();

  m_win_handle = other.m_win_handle;
  m_initialized = other.m_initialized;
  m_ref_count = other.m_ref_count;

  other.m_win_handle = nullptr;
  other.m_initialized = false;
  other.m_ref_count = nullptr;

  return *this;
}

SystemWindow::SystemWindow(const SystemWindow &other) {
  std::cout << "Copying instance of system window\n!" << std::endl;
  m_win_handle = other.m_win_handle;
  m_ref_count = other.m_ref_count;
  *m_ref_count += 1;
}

SystemWindow &SystemWindow::operator=(const SystemWindow &other) {
  std::cout << "Copy assining of system window\n!";
  auto tmp = SystemWindow(other);
  std::swap(tmp, *this);
  return *this;
}

void SystemWindow::init(const SystemWindowConfig &config) {

  std::cout << "Inialising system window object!\n" << std::endl;

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
  m_initialized = true;
  m_win_handle = win;

  WindowSystem::register_handle(win, *this);
}

void SystemWindow::swap_buffers() {
  // std::cout << "win_handle: " << m_win_handle << "\n";
  glfwSwapBuffers(m_win_handle);
}

void SystemWindow::swap(SystemWindow &a, SystemWindow &b) {
  std::swap(a.m_initialized, b.m_initialized);
  std::swap(a.m_ref_count, b.m_ref_count);
  std::swap(a.m_win_handle, b.m_win_handle);
}

void SystemWindow::bind_context() { glfwMakeContextCurrent(m_win_handle); }

SystemWindowBuilder WindowSystem::new_window(std::string window_purpose) {
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
  WindowSystem::instance().register_window(m_config.purpose, win);
  std::cout << "I built the thing damn it!\n";
  return std::move(win);
}

WindowSystem::WindowSystem() {}

WindowSystem &WindowSystem::instance() {
  static WindowSystem inst;
  return inst;
}

void WindowSystem::redirect_inputs(GLFWwindow *handle, int keycode,
                                   int scancode, int key_state_native,
                                   int mods) {

  KeyCode key{keycode};

  auto &game = Game::instance();
  auto &ws = WindowSystem::instance();
  auto win = ws.find_system_window(handle);

  if (!win) {
    throw EventOnUnregisteredWindow("Event received on unregistered window."
                                    "The window must have been created without "
                                    "the acknowledment of WindowSystem.");
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

  if (key_state_native == GLFW_PRESS || key_state_native == GLFW_RELEASE) {
    auto key_state =
        key_state_native == GLFW_PRESS ? KeyState::PRESSED : KeyState::RELEASED;
    KeyEvent ke{*win, key, key_state};

    if (auto key_bind = game.m_keymap.find(ke);
        key_bind != std::end(game.m_keymap)) {
#ifdef DEBUG_MESSAGES
      std::cout << "Running action!\n";
#endif
      std::cout << "Key bind: " << key_bind->first.key << std::endl;
      // Run the action.
      (*(key_bind->second))();
    }
  }
}

std::optional<SystemWindow>
WindowSystem::find_system_window(SystemWindowHandle handle) {
  if (auto win = m_sw_handle_lookup.find(handle);
      win != m_sw_handle_lookup.end()) {
    return win->second;
  }
  return {};
}

void WindowSystem::poll_events() { glfwPollEvents(); }

bool SystemWindow::operator<(const SystemWindow &other) const {
  if (other.m_win_handle == m_win_handle && other.m_ref_count != m_ref_count) {
    std::cout << "WARNING: SystemWindow with multiple ref conters!";
  }
  return m_win_handle < other.m_win_handle;
}

bool SystemWindow::operator==(const SystemWindow &other) const {
  if (other.m_win_handle == m_win_handle && other.m_ref_count != m_ref_count) {
    std::cout << "WARNING: SystemWindow with multiple counters!";
  }
  return m_win_handle == other.m_win_handle;
}

bool SystemWindow::operator>(const SystemWindow &other) const {
  if (other.m_win_handle == m_win_handle && other.m_ref_count != m_ref_count) {
    std::cout << "WARNING: SystemWindow with multiple counters!";
  }
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

WindowSystem::~WindowSystem() { glfwTerminate(); }

void WindowSystem::register_window(std::string win_purpose, SystemWindow win) {
  auto &inst = WindowSystem::instance();
  inst.m_system_windows[win_purpose] = win;
}

void WindowSystem::unregister_window(std::string purpose) {
  auto &inst = WindowSystem::instance();
  auto it = inst.m_system_windows.find(purpose);
  if (it != inst.m_system_windows.end()) {
    inst.m_system_windows.erase(it);
  }
}

void WindowSystem::purge_window(SystemWindow win) {
  auto &inst = WindowSystem::instance();
  for (auto it = inst.m_system_windows.begin();
       it != inst.m_system_windows.end();) {
    if (it->second == win) {
      it = inst.m_system_windows.erase(it);
    } else {
      it++;
    }
  }
}

void WindowSystem::register_handle(SystemWindowHandle handle,
                                   SystemWindow win) {
  auto &inst = WindowSystem::instance();
  inst.m_sw_handle_lookup[handle] = win;
}

void WindowSystem::window_resized_cb(SystemWindowHandle handle, uint32_t w,
                                     uint32_t h) {
  auto &inst = WindowSystem::instance();
  auto it = inst.m_sw_handle_lookup.find(handle);
  if (it != inst.m_sw_handle_lookup.end()) {
    it->second.resize_cb(it->second, w, h);
  }
}

void SystemWindow::ResizeCB::operator()(SystemWindow win, uint32_t width,
                                        uint32_t height) {
  if (m_resize_cb) {
    m_resize_cb(win, width, height);
  }
}

void SystemWindow::ResizeCB::operator=(SystemWindow::ResizeCB::CBfunc func) {
  m_resize_cb = func;
}
