#include "gfx.hxx"
#include "geom.hxx"
#include "gl.hxx"
#include "iterator.hxx"
#include "utility.hxx"
#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include "iterator.hxx"

#define EXPR_LOG(expr) std::clog << (#expr) << " = " << (expr) << std::endl;
#define ITER_LOG(container)                                                    \
  {                                                                            \
    std::clog << (#container) << " = ";                                        \
    ITER_LOG_INNER(container);                                                 \
    std::cout << std::endl;                                                    \
  }

void ITER_LOG_INNER(auto container) {
  std::clog << "{ ";
  auto iter = container.begin();
  if (iter != container.end()) {
    std::clog << *iter;
    iter++;
  }
  for (; iter != container.end(); iter++) {
    std::clog << ", " << *iter;
  }
  std::clog << " }";
}

gfx::GraphicalSettings::GraphicalSettings() : m_window_res{800, 600} {}

gfx::GraphicalSettings::GraphicalSettings(int res[2])
    : m_window_res{res[0], res[1]} {}

gfx::GraphicalSettings::GraphicalSettings(int ww, int wh)
    : m_window_res{ww, wh} {}

// FIXME: Check if settings are equal if yes than return don't update "changed"
// status.
gfx::GraphicalSettings::GraphicalSettings(const GraphicalSettings &other) {
  std::memcpy(m_window_res, other.m_window_res, sizeof(m_window_res));
  m_has_changed = true;
}

void gfx::GraphicalSettings::set_resolution(int res[2]) {
  m_has_changed = true;
  m_window_res[0] = res[0];
  m_window_res[1] = res[1];
}

void gfx::GraphicalSettings::set_resolution(int ww, int wh) {
  m_has_changed = true;
  m_window_res[0] = ww;
  m_window_res[1] = wh;
}

bool gfx::GraphicalSettings::has_changed() const { return m_has_changed; }

void gfx::GraphicalSettings::acknowledge_change() { m_has_changed = false; }

gfx::Graphics::Graphics() : m_settings() {
  std::cout << "Creating graphics!\n";
}

gfx::Graphics::Graphics(GraphicalSettings settings)
    : m_settings(std::move(settings)) {
  std::cout << "Creating graphics with settings!\n";
}

std::string stringify_shader_type(GLenum shader_type) {
  switch (shader_type) {
  case GL_VERTEX_SHADER:
    return "GL_VERTEX_SHADER";
  case GL_FRAGMENT_SHADER:
    return "GL_FRAGMENT_SHADER";
  case GL_GEOMETRY_SHADER:
    return "GL_GEOMETRY_SHADER";
  case GL_TESS_CONTROL_SHADER:
    return "GL_TESS_CONTROL_SHADER";
  case GL_TESS_EVALUATION_SHADER:
    return "GL_TESS_EVALUATION_SHADER";
  case GL_COMPUTE_SHADER:
    return "GL_COMPUTE_SHADER";
  default:
    throw std::invalid_argument("Unknown shader type: " +
                                std::to_string(shader_type));
  }
}

GLuint gfx::Graphics::compile_shader(GLenum shader_type,
                                     const std::string &source,
                                     const std::string &filename) {
  GLuint shader = glCreateShader(shader_type);

  if (shader == 0) {
    throw ShaderCompileError(std::string("Failed to create shader object of type ") +
                             stringify_shader_type(shader_type) +
                             "(FATAL ERROR)");
  }

  const char *source_cstr = source.c_str();
  GLint length = static_cast<GLint>(source.size());
  glShaderSource(shader, 1, &source_cstr, &length);
  glCompileShader(shader);

  // Check compilation status
  GLint success = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::array<char, 1024> log;
    std::fill(begin(log), end(log), 0);
    glGetShaderInfoLog(shader, log.size(), NULL, &log[0]);
    throw ShaderCompileError(filename + ": " + &log[0]);
  }

  return shader;
}

template <typename Iterator>
GLuint gfx::Graphics::link_shader_program(Iterator begin, Iterator end) {
  static_assert(
      std::is_same_v<typename std::iterator_traits<Iterator>::value_type,
                     GLuint>,
      "'link_shader_program' takes iterator over GLuint values.");

  std::array<char, 1024> log;
  std::fill(log.begin(), log.end(), 0);

  GLuint shader_program = glCreateProgram();

  if (shader_program == 0) {
    throw ShaderProgramLinkingError(
        "Failed to create shader program object (FATAL ERROR)");
  }

  std::for_each(begin, end, [=](GLuint shader_id) {
    glAttachShader(shader_program, shader_id);
  });

  glLinkProgram(shader_program);

  std::for_each(begin, end, [=](GLuint shader_id) {
    glDetachShader(shader_program, shader_id);
  });

  GLint success = GL_FALSE;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(shader_program, 1024, NULL, &log[0]);
    throw ShaderProgramLinkingError(&log[0]);
  }

  return shader_program;
}

void gfx::Graphics::init_shaders() {
  auto vertex_shader_path = "./shaders/simple.vertex.glsl";
  auto fragment_shader_path = "./shaders/simple.fragment.glsl";
  auto vertex_shader_text = read_text_file(vertex_shader_path).value_or("");
  if (vertex_shader_text.size() == 0) {
    throw std::runtime_error("Missing vertex shader source!");
  }

  auto fragment_shader_text = read_text_file(fragment_shader_path).value_or("");
  if (fragment_shader_text.size() == 0) {
    throw std::runtime_error("Missing fragment shader source!");
  }

  std::cout << "Vertex Shader = " << vertex_shader_text << std::endl;

  std::array<GLuint, 2> shaders = {
      compile_shader(GL_VERTEX_SHADER, vertex_shader_text, vertex_shader_path),
      compile_shader(GL_FRAGMENT_SHADER, fragment_shader_text,
                     fragment_shader_path)};

  GLuint main_shader_program_id =
      link_shader_program(shaders.begin(), shaders.end());

  for (auto shader : shaders) {
    glDeleteShader(shader);
  }

  std::cout << "main_shader = " << main_shader_program_id << std::endl;
  this->m_main_shader_id = main_shader_program_id;
  std::cout << "m_main_shader = " << this->m_main_shader_id << std::endl;
}

gfx::Graphics::~Graphics() {
  std::cout << "Destroying graphics!\n";
  destroy();
}

void gfx::Graphics::init() {
  std::cout << "Initing graphics!\n";
  init_shaders();
  m_gpu.init();
}

void gfx::Graphics::destroy() {}

gfx::GraphicalSettings gfx::Graphics::get_settings() const {
  return m_settings;
}

void gfx::Graphics::update_settings(GraphicalSettings settings) {
  m_settings = settings;
}

gfx::GPU::GPU() : cube_mesh() {}

void GLAPIENTRY gl_error_callback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message,
                                  const void *userParam) {
  std::cerr << "OpenGL Debug Message:\n"
            << "\tSource: " << source << "\n"
            << "\tType: " << type << "\n"
            << "\tID: " << id << "\n"
            << "\tSeverity: " << severity << "\n"
            << "\tMessage: " << message << std::endl;
}

void gfx::GPU::init() {

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&gl_error_callback, 0);
  // init_square();
  init_triangle();
  // init_cube();
}

void gfx::GPU::init_square() {
  square_mesh.init();
  square_mesh.send_position_data(&geom::square_vertices[0],
                                 geom::square_vertices.size());
  const std::array<glm::vec4, 4> square_colors = {{{1.0, 0.0, 0.0, 1.0},
                                                   {1.0, 0.0, 0.0, 1.0},
                                                   {1.0, 0.0, 0.0, 1.0},
                                                   {1.0, 0.0, 0.0, 1.0}}};
  square_mesh.send_color_data(&square_colors[0], square_colors.size());
  square_mesh.send_index_data(&geom::square_indices[0],
                              geom::square_indices.size());
}

void gfx::GPU::init_triangle() {
  triangle_mesh.init();
  triangle_mesh.send_position_data(&geom::triangle_vertices[0],
                                   geom::triangle_vertices.size());
  const std::array<glm::vec4, 3> triangle_colors = {
      {{1.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}}};
  triangle_mesh.send_color_data(&triangle_colors[0], triangle_colors.size());
  triangle_mesh.send_index_data(&geom::triangle_indices[0],
                                geom::triangle_indices.size());
}

void gfx::GPU::init_cube() {
  cube_mesh.init();
  cube_mesh.send_position_data(&geom::cube_vertices[0],
                               geom::cube_vertices.size());
  const std::array<glm::vec4, 8> cube_colors = {{{1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0},
                                                 {1.0, 1.0, 1.0, 1.0}}};

  cube_mesh.send_color_data(&cube_colors[0], cube_colors.size());
  cube_mesh.send_index_data(&geom::cube_indices[0], geom::cube_indices.size());
}

void gfx::Graphics::draw() {
  // EXPR_LOG(m_main_shader_id);
  glUseProgram(m_main_shader_id);
  m_gpu.draw();
}

void gfx::GPU::draw() {
  // square_mesh.draw();
  triangle_mesh.draw();
  // cube_mesh.draw();
}

gfx::SimpleMesh::SimpleMesh() : m_buffers({0}), m_attribs({0, 1}) {}

void gfx::SimpleMesh::init() {
  std::cout << "Buffers::COUNT = " << SIZE(Buffers::COUNT) << std::endl;
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glGenBuffers(SIZE(Buffers::COUNT), &m_buffers[0]);
  std::cout << "m_vao = " << m_vao << std::endl;
  glBindVertexArray(0);

  // NOTE: We might want to use different shaders with the same data,
  // so just use static attribute locations.
  // for (size_t i = 0; i < SIZE(Attributes::COUNT); i++) {
  //   m_attribs[i] = glGetAttribLocation(shader, m_attrib_names[i]);
  // }
}

gfx::SimpleMesh::~SimpleMesh() {
  glDeleteBuffers(SIZE(Buffers::COUNT), &m_buffers[0]);
  glDeleteVertexArrays(1, &m_vao);
}

#define BUFFER_ID(buffer) (this->m_buffers[SIZE(Buffers::buffer)])
#define ATTRIB_ID(attrib) (this->m_attribs[SIZE(Attributes::attrib)])

void gfx::SimpleMesh::send_position_data(const glm::vec3 *data, size_t count) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, BUFFER_ID(POSITION));
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, data,
               GL_STATIC_DRAW);
  EXPR_LOG(BUFFER_ID(POSITION));
  glVertexAttribPointer(ATTRIB_ID(POSITION), 3, GL_FLOAT, GL_FALSE,
                        sizeof(glm::vec3), nullptr);
  glEnableVertexAttribArray(ATTRIB_ID(POSITION));
  EXPR_LOG(ATTRIB_ID(POSITION));
  glBindVertexArray(0);
}

void gfx::SimpleMesh::send_color_data(const glm::vec4 *data, size_t count) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, BUFFER_ID(COLOR));
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * count, data,
               GL_STATIC_DRAW);
  EXPR_LOG(ATTRIB_ID(COLOR));
  glVertexAttribPointer(ATTRIB_ID(COLOR), 4, GL_FLOAT, GL_FALSE,
                        sizeof(glm::vec4), nullptr);
  glEnableVertexAttribArray(ATTRIB_ID(COLOR));
  glBindVertexArray(0);
}

void gfx::SimpleMesh::send_index_data(const uint16_t *data, size_t count) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFER_ID(INDEX));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * count, data,
               GL_STATIC_DRAW);
  m_index_count = count;
  glBindVertexArray(0);
}

void gfx::SimpleMesh::draw() {
  glFrontFace(GL_CCW);
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

#undef ATTRIB_ID
#undef BUFFER_ID
#undef ITER_LOG
#undef EXPR_LOG

GLuint Shader::id() const {
  return m_id;
}

Shader::Shader():
  m_id(0), m_ref_count(1) {}

Shader Shader::from_file(GLuint type, const std::string& path) {
  auto shader = Shader();

  auto shader_text = read_text_file(path).value_or("");
  if (shader_text.size() == 0) {
    throw std::runtime_error(std::string("Missing of type") + stringify_shader_type(type) + "source!");
  }

  shader.m_id = gfx::Graphics::compile_shader(type, shader_text, path);

  return shader;
}

Shader Shader::from_source(GLuint type, const std::string& source) {
  auto shader = Shader();

  shader.m_id = gfx::Graphics::compile_shader(type, source, "<buffer>");

  return shader;
}

Shader::~Shader() {
  m_ref_count -= 1;
  if(m_ref_count < 0) {
    std::cerr << "Ref count underflow in Shader class!\n";
  }
  if(m_ref_count == 0) {
    glDeleteShader(m_id);
  }
}

void Shader::swap(Shader& rhs, Shader& lhs) {
  std::swap(rhs.m_id, lhs.m_id);
  std::swap(rhs.m_ref_count, lhs.m_ref_count);
}

Shader::Shader(const Shader& other): m_id(other.m_id), m_ref_count(other.m_ref_count+1) {}
Shader& Shader::operator=(const Shader& other) {
  auto shader = Shader(other);
  std::swap(shader, *this);
  return *this;
}

GLuint ShaderProgram::id() const {
  return m_id;
}

ShaderProgram::ShaderProgram():
  m_id(0), m_ref_count(1) {}

void ShaderProgram::swap(ShaderProgram& rhs, ShaderProgram& lhs) {
  std::swap(rhs.m_id, lhs.m_id);
  std::swap(rhs.m_ref_count, lhs.m_ref_count);
}

ShaderProgram::~ShaderProgram() {
  m_ref_count -= 1;
  if(m_ref_count < 0) {
    std::cerr << "Ref count underflow in ShaderProgram class!\n";
  }
  if(m_ref_count == 0) {
    glDeleteProgram(m_id);
  }
}

ShaderProgram::ShaderProgram(const ShaderProgram& other): m_id(other.m_id), m_ref_count(other.m_ref_count+1) {}
ShaderProgram& ShaderProgram::operator=(const ShaderProgram& other) {
  auto program = ShaderProgram(other);
  std::swap(program, *this);
  return *this;
}

template <typename Iterator> ShaderProgram ShaderProgram::link(Iterator begin, Iterator end) {

  auto program = ShaderProgram();
  auto extract_id = [](Shader& shader) { return shader.id(); };
  auto mapped_begin = make_mapping_iterator(begin, extract_id);
  auto mapped_end = make_mapping_iterator(end, extract_id);

  program.m_id = gfx::Graphics::link_shader_program(mapped_begin, mapped_end);

  return program;
}
