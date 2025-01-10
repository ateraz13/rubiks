#ifndef WINDOW_HXX
#define WINDOW_HXX
#include "gl.hxx"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <map>
#include <optional>
#include <string>
#include <tuple>

using SystemWindowHandle = GLFWwindow *;
using KeyboardKey = int;

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
  void swap_buffers();

  static void swap(SystemWindow &, SystemWindow &);

  bool operator<(const SystemWindow& other) const;
  bool operator==(const SystemWindow& other) const;
  bool operator>(const SystemWindow& other) const;

private:
  SystemWindow();
  void init(const SystemWindowConfig &win);

  SystemWindowHandle m_win_handle = nullptr;
  int *m_ref_count = nullptr;
  bool m_initialized = false;

  friend class SystemWindowBuilder;
  friend class WindowSystem;
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

  static WindowSystem &instance();

  WindowSystem(const WindowSystem &other) = delete;
  WindowSystem &operator=(const WindowSystem &other) = delete;
  WindowSystem(WindowSystem &&other) = delete;

  static void redirect_inputs(GLFWwindow *handle, KeyboardKey key, int scancode,
                              int key_state_native, int mods);

  void poll_events();

private:
  WindowSystem();

  std::map<std::string, SystemWindow> m_system_windows;
};

enum struct KeyState { PRESSED, RELEASED };

struct KeyEvent {
  SystemWindow window;
  KeyboardKey key;
  KeyState state;

  bool operator<(const KeyEvent& other) const;
  bool operator==(const KeyEvent& other) const;
  bool operator>(const KeyEvent& other) const;
};


#endif // WINDOW_HXX
