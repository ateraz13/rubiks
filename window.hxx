#ifndef WINDOW_HXX
#define WINDOW_HXX
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <map>
#include <optional>
#include <string>
#include <tuple>

using SystemWindowHandle = GLFWwindow *;

struct SystemWindowConfig {
public:
  struct GLVersion {
    int major = 4, minor = 5;
  };
  int width = 800, height = 600;
  std::optional<GLVersion> opengl_version = {};
  std::string title = "No title";
  std::string purpose = "undefined";

private:
  SystemWindowConfig();
  friend class SystemWindowBuilder;
};

class SystemWindow {
public:
  void resize(int w, int h);
  glm::ivec2 size();

  SystemWindow(const SystemWindow &other);
  SystemWindow &operator=(const SystemWindow &other);

  ~SystemWindow();

  void bind_context();

  static void swap(SystemWindow &, SystemWindow &);

private:
  SystemWindow();
  void init(const SystemWindowConfig &win);

  SystemWindowHandle m_win_handle = nullptr;
  int *m_ref_count;
  bool m_initialized = false;
  friend class SystemWindowBuilder;
};

class SystemWindowBuilder {
public:
  SystemWindowBuilder &with_size(int w, int h);
  SystemWindowBuilder &with_opengl(int major_version, int minor_version);
  SystemWindowBuilder &with_title(const std::string &title);
  SystemWindow build();

  SystemWindowBuilder(const SystemWindowBuilder &other) = delete;
  SystemWindowBuilder &operator=(const SystemWindowBuilder &other) = delete;
  SystemWindowBuilder &operator=(SystemWindowBuilder &&other) = delete;

private:
  explicit SystemWindowBuilder(std::string purpose);
  SystemWindowBuilder();

  SystemWindowConfig m_config;
  friend class WindowSystem;
};

SystemWindowBuilder sys_window();

class WindowSystem {
public:
  std::optional<SystemWindow> find_system_window(SystemWindowHandle handle);
  SystemWindowBuilder make(std::string window_purpose);

  static WindowSystem& instance();

  WindowSystem(const WindowSystem &other) = delete;
  WindowSystem &operator=(const WindowSystem &other) = delete;
  WindowSystem(WindowSystem &&other) = delete;

private:
  WindowSystem();

  std::map<std::string, SystemWindow> m_system_windows;
};

#endif // WINDOW_HXX
