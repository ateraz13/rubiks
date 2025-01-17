#ifndef WINDOW_HXX
#define WINDOW_HXX
#include "gl.hxx"
#include "keys.hxx"
#include <GLFW/glfw3.h>
#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <optional>
#include <string>
#include <tuple>

using SystemWindowHandle = GLFWwindow *;
using KeyboardKey = KeyCode;

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
  SystemWindow(SystemWindow &&other);
  SystemWindow &operator=(SystemWindow &&other);
  SystemWindow();
  ~SystemWindow();

  struct ResizeCB {
  public:
    using CBfunc = std::function<void(SystemWindow, uint32_t, uint32_t)>;

    void operator=(CBfunc cb);
    void operator()(SystemWindow win, uint32_t width, uint32_t height);

  private:
    CBfunc m_resize_cb;
  } resize_cb;

  void bind_context();
  void swap_buffers();

  static void swap(SystemWindow &, SystemWindow &);

  bool operator<(const SystemWindow &other) const;
  bool operator==(const SystemWindow &other) const;
  bool operator>(const SystemWindow &other) const;

private:
  void init(const SystemWindowConfig &win);
  void clean_up();

  SystemWindowHandle m_win_handle = nullptr;
  int *m_ref_count = nullptr;
  bool m_initialized = false;

  friend class SystemWindowBuilder;
  friend class WindowSystem;
  friend std::ostream& operator<<(std::ostream &strm,
                                 const SystemWindow &window);
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
  SystemWindowBuilder new_window(std::string window_purpose);

  static WindowSystem &instance();
  ~WindowSystem();

  WindowSystem(const WindowSystem &other) = delete;
  WindowSystem &operator=(const WindowSystem &other) = delete;
  WindowSystem(WindowSystem &&other) = delete;

  static void redirect_inputs(GLFWwindow *handle, int keycode, int scancode,
                              int key_state_native, int mods);

  static void register_window(std::string purpose, SystemWindow win);
  static void unregister_window(std::string purpose);
  static void purge_window(SystemWindow win);
  static void poll_events();

private:
  static void register_handle(SystemWindowHandle handle, SystemWindow window);

  WindowSystem();

  static void window_resized_cb(GLFWwindow *win, uint32_t w, uint32_t h);

  std::map<SystemWindowHandle, SystemWindow> m_sw_handle_lookup;
  std::map<std::string, SystemWindow> m_system_windows;

  friend class SystemWindow;
};

enum struct KeyState { PRESSED, RELEASED };

struct KeyEvent {
  SystemWindow window;
  KeyboardKey key;
  KeyState state;

  bool operator<(const KeyEvent &other) const;
  bool operator==(const KeyEvent &other) const;
  bool operator>(const KeyEvent &other) const;
  friend std::ostream& operator<<(std::ostream& strm, const KeyEvent& event);
};

std::ostream& operator<<(std::ostream &strm, const KeyState &state);
std::ostream& operator<<(std::ostream &strm, const KeyEvent &event);
std::ostream& operator<<(std::ostream& strm, const SystemWindow& window);

#endif // WINDOW_HXX
