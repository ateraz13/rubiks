#include "window.hxx"
#include "except.hxx"
#include "game.hxx"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <optional>

static std::atomic<bool> is_glfw_initialized = false;
static std::atomic<size_t> system_window_count = 0;

SystemWindow::SystemWindow() : m_internal(nullptr) {}

SystemWindow::SystemWindow(SystemWindow &&other)
    : m_internal(other.m_internal) {
  std::cout << "Move construting system window!\n" << std::endl;

  other.m_internal = nullptr;
}

SystemWindow::~SystemWindow() { clean_up(); }

void SystemWindow::clean_up() {
  if (m_internal) {
    std::cout << "Destroying window instance!\n";
    m_internal->ref_count -= 1;
    if (m_internal->ref_count < 0) {
      std::cout << "SystemWindow: ref count below zero!\n";
    }
    if (m_internal->ref_count == 0) {
      if (m_internal->initial_config.use_imgui) {
        std::cout << "Shutting down imgui!\n";
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
      }
      std::cout << "Destroying window!\n";
      system_window_count -= 1;
      is_glfw_initialized = false;
      glfwDestroyWindow(m_internal->win_handle);
      delete m_internal;
    }
  }
}

SystemWindow &SystemWindow::operator=(SystemWindow &&other) {
  std::cout << "Move assigning system window!\n" << std::endl;

  clean_up();

  m_internal = other.m_internal;
  other.m_internal = nullptr;

  return *this;
}

SystemWindow::SystemWindow(const SystemWindow &other) {
  if (other.m_internal) {
    m_internal = other.m_internal;
    m_internal->ref_count++;
  }
}

SystemWindow &SystemWindow::operator=(const SystemWindow &other) {
  SystemWindow(std::move(*this));
  if (other.m_internal) {
    m_internal = other.m_internal;
    m_internal->ref_count++;
  }
  return *this;
}

void SystemWindow::init(const SystemWindowConfig &config) {
  { SystemWindow(std::move(*this)); } // Unreference my self

  m_internal = new SystemWindow::Internal();

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

  glfwMakeContextCurrent(win);

  if (config.opengl_version) {
    load_opengl_funcs(&glfwGetProcAddress);
  }

  if (config.use_imgui && config.opengl_version) {
    std::cout << "Init imgui!\n!";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 450");
  }

  glfwSetKeyCallback(win, &WindowSystem::redirect_inputs);
  glfwSetWindowSizeCallback(win, &WindowSystem::redirect_resize_cb);
  system_window_count += 1;
  m_internal->win_handle = win;
  m_internal->initial_config = config;
  m_internal->ref_count = 1;

  WindowSystem::register_handle(win, *this);
}

void SystemWindow::swap_buffers() {
  assert(m_internal != nullptr);
  glfwSwapBuffers(m_internal->win_handle);
}

void SystemWindow::swap(SystemWindow &a, SystemWindow &b) {
  std::swap(a.m_internal, b.m_internal);
}

void SystemWindow::bind_context() {
  assert(m_internal != nullptr);
  glfwMakeContextCurrent(m_internal->win_handle);
}

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
  return std::move(win);
}

WindowSystem::WindowSystem() {}

WindowSystem &WindowSystem::instance() {
  static WindowSystem inst;
  return inst;
}

void WindowSystem::redirect_resize_cb(GLFWwindow *handle, int width,
                                      int height) {
  auto &ws = WindowSystem::instance();
  auto win = ws.find_system_window(handle);

  std::cout << "Window size changed!\n";
  if (win) {
    std::cout << "Calling resize callback!\n";
    win->m_internal->resize_cb(*win, width, height);
  }
}

void WindowSystem::redirect_inputs(GLFWwindow *handle, int keycode,
                                   int scancode, int key_state_native,
                                   int mods) {

  KeyCode key{keycode};
  std::cout << "Keycode: " << keycode << " = " << key << std::endl;

  auto &game = Game::instance();
  auto &ws = WindowSystem::instance();
  auto win = ws.find_system_window(handle);

  if (!win) {
    throw EventOnUnregisteredWindow("Event received on unregistered window."
                                    "The window must have been created without "
                                    "the acknowledment of WindowSystem.");
  }

#ifdef DEBUG_MESSAGES
  std::cout << "Pressed a key: " << key << std::endl;
#endif

  if (key_state_native == GLFW_PRESS || key_state_native == GLFW_RELEASE) {
    auto key_state =
        key_state_native == GLFW_PRESS ? KeyState::PRESSED : KeyState::RELEASED;
    KeyEvent ke{*win, key, key_state};
    std::cout << "Keyboard event: " << ke << std::endl;

    if (auto key_bind = game.m_keymap.find(ke);
        key_bind != game.m_keymap.end()) {
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
  if (other.m_internal->win_handle == m_internal->win_handle && other.m_internal->ref_count != m_internal->ref_count) {
    std::cout << "WARNING: SystemWindow with multiple ref conters!";
  }
  return m_internal->win_handle < other.m_internal->win_handle;
}

bool SystemWindow::operator==(const SystemWindow &other) const {
  if (other.m_internal->win_handle == m_internal->win_handle && other.m_internal->ref_count != m_internal->ref_count) {
    std::cout << "WARNING: SystemWindow with multiple counters!";
  }
  return m_internal->win_handle == other.m_internal->win_handle;
}

bool SystemWindow::operator>(const SystemWindow &other) const {
  if (other.m_internal->win_handle == m_internal->win_handle && other.m_internal->ref_count != m_internal->ref_count) {
    std::cout << "WARNING: SystemWindow with multiple counters!";
  }
  return m_internal->win_handle > other.m_internal->win_handle;
}

bool KeyEvent::operator<(const KeyEvent &other) const {
  if (window == other.window) {
    if (key == other.key) {
      return state < other.state;
    }
    return key < other.key;
  }
  return window < other.window;
}

bool KeyEvent::operator==(const KeyEvent &other) const {
  return window == other.window && key == other.key && state == other.state;
}

bool KeyEvent::operator>(const KeyEvent &other) const {
  if (window == other.window) {
    if (key == other.key) {
      return state > other.state;
    }
    return key > other.key;
  }
  return window > other.window;
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

  for (auto it = inst.m_sw_handle_lookup.begin();
       it != inst.m_sw_handle_lookup.end();) {
    if (it->second == win) {
      it = inst.m_sw_handle_lookup.erase(it);
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
    it->second.m_internal->resize_cb(it->second, w, h);
  }
}

std::ostream &operator<<(std::ostream &strm, const KeyEvent &event) {
  strm << "KeyEvent { " << event.window << ", " << event.key << ", "
       << event.state << " }";
  return strm;
}

std::ostream &operator<<(std::ostream &strm, const SystemWindow &window) {
  if(window.m_internal) {
    strm << "GLFWwindow* (" << window.m_internal->win_handle << ")";
  } else {
    strm << "[Null Window]";
  }
  return strm;
}

std::ostream &operator<<(std::ostream &strm, const KeyState &state) {
  switch (state) {
  case KeyState::PRESSED:
    strm << "PRESSED";
    break;
  case KeyState::RELEASED:
    strm << "RELEASED";
    break;
  default:
    strm << "UNKNOWN_STATE";
    break;
  }
  return strm;
}

SystemWindowBuilder &SystemWindowBuilder::with_imgui() {
  m_config.use_imgui = true;
  return *this;
}

void SystemWindow::set_resize_cb(ResizeCB resize_cb) {
  assert(m_internal != nullptr);
  m_internal->resize_cb = resize_cb;
}
