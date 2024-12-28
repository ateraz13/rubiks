#include "window.hxx"
#include "except.hxx"
#include <GLFW/glfw3.h>

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

  system_window_count += 1;
}

SystemWindow::~SystemWindow() {
  if (m_initialized) {
    system_window_count -= 1;
    is_glfw_initialized = false;
    glfwDestroyWindow(m_win_handle);
    glfwTerminate();
  }
}

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

SystemWindowConfig::SystemWindowConfig() {

}

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

WindowSystem::WindowSystem() {

}

WindowSystem &WindowSystem::instance() {
  static WindowSystem inst;
  return inst;
}
