#include "gfx.hxx"
#include "game.hxx"
#include "geom.hxx"
#include "gl_calls.hxx"
#include "imgui_impl_opengl3.h"
#include "iterator.hxx"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include "utility.hxx"
#include <algorithm>
#include <array>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
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

  init_square();
  init_triangle();
  init_cube();
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

  std::vector<glm::vec3> vertices = {};
  std::vector<glm::vec3> normals = {};
  std::vector<glm::vec4> colors = {};
  std::vector<uint16_t> indices = {};
  vertices.reserve(geom::cube_vertices.size() * 6);
  normals.reserve(geom::cube_vertices.size() * 6);
  colors.reserve(geom::cube_vertices.size() * 6);

  // clang-format off
  const glm::vec3 up      = {0.0f, -1.0f, 0.0f};
  const glm::vec3 down    = {0.0f, 1.0f, 0.0f};
  const glm::vec3 left    = {-1.0f, 0.0f, 0.0f};
  const glm::vec3 right   = { 1.0f, 0.0f, 0.0f};
  const glm::vec3 forward = {0.0f, 0.0f, -1.0f};
  const glm::vec3 back    = {0.0f, 0.0f, 1.0f};
  const glm::vec4 red     = {1.0f, 0.0f, 0.0f, 1.0f};
  const glm::vec4 green   = {0.0f, 1.0f, 0.0f, 1.0f};
  const glm::vec4 blue    = {0.0f, 0.0f, 1.0f, 1.0f};
  const glm::vec4 yellow  = {1.0f, 1.0f, 0.0f, 1.0f};
  const glm::vec4 orange  = {1.0f, 0.5f, 0.0f, 1.0f};
  const glm::vec4 white   = {1.0f, 1.0f, 1.0f, 1.0f};
  const glm::vec4 black   = {0.0f, 0.0f, 0.0f, 1.0f};
  // clang-format on

  std::array<glm::vec3, 6> directions = {up, down, left, right, forward, back};
  std::array<glm::vec4, 6> pallete = {blue, green, red, orange, yellow, white};
  glm::vec3 default_normal = {0.0f, 0.0f, -1.0f};

  std::vector<glm::vec3> frame_vertices = {
      {-0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
  };

  std::vector<uint16_t> frame_indices = {0, 2, 1, 1, 2, 3};

  const auto frame_thickness = 0.2f;

  // Generate inner frame vertices based on thickness
  {
    auto v = glm::vec3(0.0f, frame_thickness, 0.0f);
    auto h = glm::vec3(frame_thickness, 0.0f, 0.0f);
    frame_vertices.reserve(4);
    frame_vertices.push_back(frame_vertices[0] + v + h);
    frame_vertices.push_back(frame_vertices[1] + v + -h);
  }

  {
    auto og_frame_vertices(frame_vertices);
    auto og_frame_indices(frame_indices);
    auto i = 4;
    for (int n = 1; n < 4; n++) {
      for (auto fi : og_frame_indices) {
        auto v = glm::rotate(glm::mat4(1.0f), n * glm::radians(90.0f),
                             glm::vec3(0.0f, 0.0f, 1.0f)) *
                 glm::vec4(og_frame_vertices[fi], 1.0f);
        frame_vertices.push_back(glm::vec3(v));
        frame_indices.push_back(i);
        i++;
      }
    }
  }

  auto frame_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

  auto ft = frame_thickness;
  std::vector<glm::vec3> main_face_vertices = {
      {-0.5f + ft, -0.5f + ft, -0.5f},
      {-0.5f + ft, 0.5f - ft, -0.5f},
      {0.5f - ft, -0.5f + ft, -0.5f},
      {0.5f - ft, 0.5f  - ft, -0.5f},
  };

  std::vector<uint16_t> main_face_indices = {0, 2, 1, 1, 2, 3};

  auto index = 0;
  auto add_new_side = [&](glm::vec4 color, glm::mat4 rot) {
    for (auto fi : frame_indices) {
      auto vertex = rot * glm::vec4(frame_vertices[fi], 1.0f);
      vertices.push_back(vertex);
      indices.push_back(index);
      colors.push_back(frame_color);
      index++;
    }
    for(auto mfi: main_face_indices) {
      auto vertex = rot * glm::vec4(main_face_vertices[mfi], 1.0f);
      vertices.push_back(vertex);
      indices.push_back(index);
      colors.push_back(color);
      index++;
    }
  };

  // Front side
  add_new_side(red, glm::mat4(1.0f));
  // Left Right
  add_new_side(white,
               glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f)));
  // Back side
  add_new_side(orange,
               glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f)));
  // Right side
  add_new_side(yellow,
               glm::rotate(glm::mat4(1.0f), glm::radians(270.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f)));

  // Top side
  add_new_side(green,
               glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                           glm::vec3(1.0f, 0.0f, 0.0f)));
  // Bottom
  add_new_side(blue,
               glm::rotate(glm::mat4(1.0f), glm::radians(270.0f),
                           glm::vec3(1.0f, 0.0f, 0.0f)));

  cube_mesh.send_position_data(&vertices[0], vertices.size());
  // const std::array<glm::vec4, 8> cube_colors = {
  //     {green, green, blue, blue, red, red, yellow, yellow}};

  cube_mesh.send_color_data(&colors[0], colors.size());
  cube_mesh.send_index_data(&indices[0], indices.size());
}

void gfx::Graphics::draw() {
  // EXPR_LOG(m_main_shader->id());
  auto viewport_size = m_viewport_size.load();
  dglViewport(0, 0, viewport_size.x, viewport_size.y);
  m_main_shader->use();
  m_gpu.draw();
}

void gfx::GPU::draw() {

  int size = 3;
  float distance = 25.0f;
  float spacing = 0.90f;

  for (int z = 0; z < size; z++) {
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f), glm::vec3(-2.0 + x * spacing, -2.0 + y * spacing,
                                       -2.0f + z * spacing));
        glm::mat4 projection = glm::perspective(glm::pi<float>() * 0.25f,
                                                4.0f / 3.0f, 0.1f, 100.f);
        glm::mat4 view = glm::translate(
            glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -std::abs(distance)));
        auto time = Game::instance().current_time();
        view = glm::rotate(view, static_cast<float>(glm::pi<double>() * time),
                           glm::vec3(0.0f, 1.0f, 0.0f));

        view = glm::rotate(view,
                           static_cast<float>(glm::pi<double>() * time *
                           0.5), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 mvp = projection * view * model;
        cube_mesh.send_mvp(mvp);
        cube_mesh.draw();
        for (int n = 0; n < 4; n++) {
        }
      }
    }
  }

  // square_mesh.send_mvp(mvp);
  // square_mesh.draw();
  // triangle_mesh.send_mvp(mvp);
  // triangle_mesh.draw();
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
  dglBindVertexArray(m_vao);
  dglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id(BufferType::INDEX));
  dglBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * count, data,
                GL_STATIC_DRAW);
  m_index_count = count;
  dglBindVertexArray(0);
}

void gfx::SimpleMesh::draw() {
  dglBindVertexArray(m_vao);
  dglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id(BufferType::INDEX));
  glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_SHORT, nullptr);
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
