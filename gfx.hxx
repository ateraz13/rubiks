#ifndef GFX_HXX
#define GFX_HXX
#include "const.hxx"
#include "shader.hxx"
#include <array>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <atomic>

namespace gfx {

template <typename T> constexpr size_t SIZE(T x) {
  return static_cast<size_t>(x);
}

/* NOTE: The graphical settings are stored in a separate class because
  I wanted to decouple where the settings from where its used. This way
 it can be modified somewhere else in the code and than simply applied to
the graphics subsystem */
class GraphicalSettings {
public:
  GraphicalSettings();
  GraphicalSettings(int res[2]);
  GraphicalSettings(int ww, int wh);
  GraphicalSettings(const GraphicalSettings &other);

  void set_resolution(int w, int h);
  void set_resolution(int resolution[2]);

  void load_from_disk();
  void save_settings() const;

  bool has_changed() const;
  void acknowledge_change();

private:
  int m_window_res[2];
  bool m_has_changed;
};

class Graphics;

struct SimpleMesh {
public:
  enum struct BufferType { POSITION = 0, COLOR, INDEX, COUNT };
  enum struct AttribType { POSITION, COLOR, COUNT };
  enum struct UniformType { MVP, COUNT };

  void init();
  void send_mvp(const glm::mat4 &mvp);
  void send_position_data(const glm::vec3 *data, size_t count);
  void send_color_data(const glm::vec4 *data, size_t count);
  void send_index_data(const uint16_t *data, size_t count);
  void draw();
  SimpleMesh();
  ~SimpleMesh();

  GLuint buffer_id(BufferType buffer);
  GLuint attrib_id(AttribType attrib);
  GLuint uniform_id(UniformType uniform);

private:
  const std::array<const char *, SIZE(AttribType::COUNT)> m_attrib_names = {
      "position", "color"};
  std::array<GLuint, SIZE(BufferType::COUNT)> m_buffers = {0};
  const std::array<GLuint, SIZE(AttribType::COUNT)> m_attribs = {0, 1};

  const std::array<GLuint, SIZE(UniformType::COUNT)> m_uniforms = {25};
  GLuint m_vao = 0;
  size_t m_index_count = 0;
};

struct GPU {
public:
  SimpleMesh triangle_mesh;
  SimpleMesh square_mesh;
  SimpleMesh cube_mesh;

  void init();
  void draw();

  GPU(const GPU &other) = delete;
  GPU &operator=(const GPU &other) = delete;

private:
  explicit GPU();
  void init_cube();
  void init_triangle();
  void init_square();
  friend Graphics;
};

class Graphics {
public:
  Graphics();
  Graphics(GraphicalSettings settings);
  ~Graphics();
  Graphics(const Graphics &other) = delete;
  Graphics &operator=(const Graphics &other) = delete;

  void init();
  void destroy();

  GraphicalSettings get_settings() const;
  void update_settings(GraphicalSettings settings);
  bool has_settings_changed() const;

  template <typename Iterator>
  static GLuint link_shader_program(Iterator begin, Iterator end);

  void init_shaders();
  void draw();
  void viewport_size(int width, int height);
  void viewport_size(glm::ivec2 size);
  glm::ivec2 viewport_size() const;

private:
  std::atomic<glm::ivec2> m_viewport_size = {{MAIN_WINDOW_DEFAULT_WIDTH, MAIN_WINDOW_DEFAULT_HEIGHT}};
  GraphicalSettings m_settings;
  std::optional<ShaderProgram> m_main_shader = std::nullopt;
  GPU m_gpu;
};

} // namespace gfx

#endif // GFX_HXX
