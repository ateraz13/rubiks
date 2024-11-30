#ifndef GFX_HXX
#define GFX_HXX
#include "gl.hxx"
#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace gfx {

constexpr auto SIZE(auto x) -> size_t { return static_cast<size_t>(x); }

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
  enum struct Buffers { POSITION = 0, COLOR, INDEX, COUNT };
  enum struct Attributes { POSITION, COLOR, COUNT };

  void init();
  void send_position_data(const glm::vec3 *data, size_t count);
  void send_color_data(const glm::vec4 *data, size_t count);
  void send_index_data(const uint16_t *data, size_t count);
  void draw();

  SimpleMesh();
  ~SimpleMesh();

private:
  const std::array<const char *, SIZE(Attributes::COUNT)> m_attrib_names = {
      "position", "color"};
  std::array<GLuint, SIZE(Buffers::COUNT)> m_buffers = {0};
  const std::array<GLuint, SIZE(Attributes::COUNT)> m_attribs = {0, 1};
  GLuint m_vao = 0;
  size_t m_index_count = 0;
  ;
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

  /**
   * @brief Links a shader program using the specified shaders.
   *
   * This function creates a new OpenGL shader program and attaches the provided
   * shaders to it. After linking the program, all shaders are detached to clean
   * up resources. If the linking fails, an exception is thrown with the error
   * log.
   *
   * @tparam Iterator A forward iterator type pointing to GLuint values, which
   * represent shader IDs.
   * @param begin Iterator to the beginning of a range of shader IDs to attach.
   * @param end Iterator to the end of a range of shader IDs to attach.
   * @throws ShaderProgramLinkingError If the shader program fails to link, an
   * exception is thrown containing the error log.
   * @return GLuint The ID of the successfully linked shader program.
   *
   * @note The input shaders must be valid, compiled shaders. It is the caller's
   * responsibility to delete the returned shader program using
   * `glDeleteProgram` when no longer needed.
   *
   * @warning Ensure that all shaders in the range are compatible (e.g., vertex
   * and fragment shaders) and meet OpenGL requirements for program linking.
   *
   * @example
   * @code
   * std::array<GLuint, 2> shaders = {vertex_shader_id, fragment_shader_id};
   * GLuint program_id = link_shader_program(shaders.begin(), shaders.end());
   * @endcode
   */
  template <typename Iterator>
  static GLuint link_shader_program(Iterator begin, Iterator end);

  /**
   * Compiles a shader of the specified type from the given source code.
   * @param shader_type OpenGL shader type (e.g., GL_VERTEX_SHADER).
   * @param source Shader source code as a string.
   * @param filename Name of the source file for error reporting.
   * @throws ShaderCompileError on compilation failure.
   * @return GLuint ID of the compiled shader.
   */
  static GLuint compile_shader(GLenum shader_type, const std::string &source,
                               const std::string &filename);
  void init_shaders();
  void draw();

private:
  GraphicalSettings m_settings;
  GLuint m_main_shader_id;
  GPU m_gpu;
};

std::optional<GLuint> load_shader_from_disk(const std::string &vshader,
                                            const std::string &fshader);
} // namespace gfx

class ShaderCompileError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class ShaderProgramLinkingError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

#endif // GFX_HXX
