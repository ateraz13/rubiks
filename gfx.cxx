#include "gfx.hxx"
#include "game.hxx"
#include "geom.hxx"
#include "gl.hxx"
#include "gl_calls.hxx"
#include "iterator.hxx"
#include "utility.hxx"
#include <algorithm>
#include <array>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

const char *gl_error_string(GLuint err) {
  switch (err) {
  case GL_NO_ERROR:
    return "No error has been recorded.";
    break;
  case GL_INVALID_ENUM:
    return "An unacceptable value is specified for an enumerated "
           "argument. The offending command is ignored and has no other "
           "side effect than to set the error flag.";
    break;
  case GL_INVALID_VALUE:
    return "A numeric argument is out of range. The offending command is "
      "ignored and has no other side effect than to set the error flag.";
    break;
  case GL_INVALID_OPERATION:
    return "The specified operation is not allowed in the current state. "
           "The offending command is ignored and has no other side "
           "effect than to set the error flag.";
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "The framebuffer object is not complete. The offending command is "
           "ignored and has no other side effect than to set the error flag.";
    break;
  case GL_OUT_OF_MEMORY:
    return "There is not enough memory left to execute the command. The "
           "state of the GL is undefined, except for the state of the "
           "error flags, after this error is recorded.";
    break;
  case GL_STACK_UNDERFLOW:
    return "An attempt has been made to perform an operation that would "
           "cause an internal stack to underflow.";
    break;
  case GL_STACK_OVERFLOW:
    return "An attempt has been made to perform an operation that would "
           "cause an internal stack to overflow.";
    break;
  default:
    return "Unknown OpenGL error.";
    break;
  }
}

void precall_callback(const char *source_file, int line_num,
                      const char *func_name) {}

void postcall_callback(const char *source_file, int line_num,
                       const char *func_name) {

  auto err = glGetError();


  std::cout << gl_error_string(err) << "\n";

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

void gfx::Graphics::init_shaders() {
  auto vertex_shader_path = "./shaders/simple.vertex.glsl";
  auto fragment_shader_path = "./shaders/simple.fragment.glsl";

  std::vector<Shader> shaders = {
      Shader::from_file(GL_VERTEX_SHADER, vertex_shader_path),
      Shader::from_file(GL_FRAGMENT_SHADER, fragment_shader_path)};

  auto main_shader_program =
      ShaderProgram::link(shaders.begin(), shaders.end());

  std::cout << "main_shader = " << main_shader_program.id() << std::endl;
  this->m_main_shader = main_shader_program;
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
  // EXPR_LOG(m_main_shader->id());
  auto viewport_size = m_viewport_size.load();
  dglViewport(0, 0, viewport_size.x, viewport_size.y);
  m_main_shader->use();
  m_gpu.draw();
}

void gfx::GPU::draw() {

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 projection =
      glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
  glm::mat4 view =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
  auto time = Game::instance().current_time();
  view = glm::rotate(view, static_cast<float>(glm::pi<double>() * time),
                     glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 mvp = projection * view * model;

  // square_mesh.send_mvp(mvp);
  // square_mesh.draw();
  triangle_mesh.send_mvp(mvp);
  triangle_mesh.draw();
  // cube_mesh.send_mvp(mvp);
  // cube_mesh.draw();
}

gfx::SimpleMesh::SimpleMesh() {}

void gfx::SimpleMesh::init() {
  std::cout << "Buffers::COUNT = " << SIZE(BufferType::COUNT) << std::endl;
  dglGenVertexArrays(1, &m_vao);
  dglBindVertexArray(m_vao);
  dglGenBuffers(m_buffers.size(), &m_buffers[0]);
  dglBindVertexArray(0);
}

void gfx::SimpleMesh::send_mvp(const glm::mat4 &mat) {
  glUniformMatrix4fv(uniform_id(UniformType::MVP), 1, GL_FALSE, &mat[0][0]);
}

gfx::SimpleMesh::~SimpleMesh() {
  dglDeleteBuffers(SIZE(BufferType::COUNT), &m_buffers[0]);
  dglDeleteVertexArrays(1, &m_vao);
}

void gfx::SimpleMesh::send_position_data(const glm::vec3 *data, size_t count) {
  dglBindVertexArray(m_vao);
  dglBindBuffer(GL_ARRAY_BUFFER, buffer_id(BufferType::POSITION));
  dglBufferData(GL_ARRAY_BUFFER, sizeof(*data) * count, data, GL_STATIC_DRAW);
  for (auto p = data; p < data + count; p++) {
    std::cout << "{" << p->x << ", " << p->y << ", " << p->z << "}\n";
  }
  dglEnableVertexAttribArray(attrib_id(AttribType::POSITION));
  EXPR_LOG(buffer_id(BufferType::POSITION));
  dglVertexAttribPointer(attrib_id(AttribType::POSITION), 3, GL_FLOAT, GL_FALSE,
                         0, nullptr);
  EXPR_LOG(attrib_id(AttribType::POSITION));

  dglBindVertexArray(0);
}

void gfx::SimpleMesh::send_color_data(const glm::vec4 *data, size_t count) {
  dglBindVertexArray(m_vao);
  dglBindBuffer(GL_ARRAY_BUFFER, buffer_id(BufferType::COLOR));
  dglBufferData(GL_ARRAY_BUFFER, sizeof(*data) * count, data, GL_STATIC_DRAW);
  EXPR_LOG(attrib_id(AttribType::COLOR));
  dglEnableVertexAttribArray(attrib_id(AttribType::COLOR));
  dglVertexAttribPointer(attrib_id(AttribType::COLOR), 4, GL_FLOAT, GL_FALSE, 0,
                         nullptr);
  dglBindVertexArray(0);
}

void gfx::SimpleMesh::send_index_data(const uint16_t *data, size_t count) {
  // dglBindVertexArray(m_vao);
  // dglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id(BufferType::INDEX));
  // dglBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * count, data,
  //               GL_STATIC_DRAW);
  // m_index_count = count;
  // dglBindVertexArray(0);
}

void gfx::SimpleMesh::draw() {
  dglBindVertexArray(m_vao);
  // dglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id(BufferType::INDEX));
  dglBindBuffer(GL_ARRAY_BUFFER, buffer_id(BufferType::POSITION));
  dglDrawArrays(GL_TRIANGLES, 0, 3);
  dglBindVertexArray(0);
}

GLuint gfx::SimpleMesh::buffer_id(BufferType buffer) {
  return m_buffers[SIZE(buffer)];
}

GLuint gfx::SimpleMesh::attrib_id(AttribType attrib) {
  return m_attribs[SIZE(attrib)];
}

GLuint gfx::SimpleMesh::uniform_id(UniformType uniform) {
  return m_uniforms[SIZE(uniform)];
}

void gfx::Graphics::viewport_size(int width, int height) {
  m_viewport_size.store(glm::ivec2(width, height));
}

void gfx::Graphics::viewport_size(glm::ivec2 size) {
  m_viewport_size.store(glm::ivec2(size));
}

glm::ivec2 gfx::Graphics::viewport_size() const {
  return m_viewport_size.load();
}
