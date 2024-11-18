#include "gfx.hxx"
#include "geom.hxx"
#include "utility.hxx"
#include <array>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#define EXPR_LOG(expr)  std::clog << (#expr) << " = " << (expr) << std::endl;
#define ITER_LOG(container) {std::clog << (#container) << " = "; ITER_LOG_INNER(container); std::cout << std::endl;}

void ITER_LOG_INNER(auto container) {
  std::clog << "{ ";
  auto iter = container.begin();
  if(iter!=container.end()) {
    std::clog << *iter;
    iter++;
  }
  for(; iter != container.end(); iter++) {
    std::clog << ", " << *iter ;
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
  auto vertex_shader_text = read_text_file(vertex_shader_path).value_or("");
  if (vertex_shader_text.size() == 0) {
    throw std::runtime_error("Missing vertex shader source!");
  }

  auto fragment_shader_text = read_text_file(fragment_shader_path).value_or("");
  if (fragment_shader_text.size() == 0) {
    throw std::runtime_error("Missing fragment shader source!");
  }

  std::cout << "Vertex Shader = " << vertex_shader_text << std::endl;

  const char *shaders[] = {vertex_shader_text.c_str(),
                           fragment_shader_text.c_str()};

  std::array<char, 1024> log_buf;
  auto check_shader = [&](const std::string &fn, GLuint shader) {
    std::stringstream ss;
    std::fill(begin(log_buf), end(log_buf), 0);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, &log_buf[0]);
      ss << "\n" << fn << ":" << &log_buf[0];
      throw ShaderCompileError(ss.str());
    }
  };

  auto check_shader_program = [&](GLuint program) {
    std::fill(begin(log_buf), end(log_buf), 0);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(program, 1024, NULL, &log_buf[0]);
      throw ShaderProgramLinkingError(&log_buf[0]);
    }
  };

  const GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_id, 1, &shaders[0], NULL);
  glCompileShader(vertex_shader_id);

  check_shader(vertex_shader_path, vertex_shader_id);

  const GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_id, 1, &shaders[1], NULL);
  glCompileShader(fragment_shader_id);

  check_shader(fragment_shader_path, fragment_shader_id);

  const GLuint main_shader_program_id = glCreateProgram();
  glAttachShader(main_shader_program_id, vertex_shader_id);
  glAttachShader(main_shader_program_id, fragment_shader_id);
  glLinkProgram(main_shader_program_id);

  check_shader_program(main_shader_program_id);

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

void gfx::GPU::init() {
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
  square_mesh.draw();
  triangle_mesh.draw();
  cube_mesh.draw();
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

void gfx::SimpleMesh::send_position_data(const glm::vec3 *data,
                                               size_t count) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, BUFFER_ID(POSITION));
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, data,
               GL_STATIC_DRAW);
  glVertexAttribPointer(ATTRIB_ID(POSITION), 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
  glEnableVertexAttribArray(ATTRIB_ID(POSITION));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gfx::SimpleMesh::send_color_data(const glm::vec4 *data,
                                            size_t count) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, BUFFER_ID(COLOR));
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * count, data,
               GL_STATIC_DRAW);
  EXPR_LOG(ATTRIB_ID(COLOR));
  glVertexAttribPointer(ATTRIB_ID(COLOR), 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
  glEnableVertexAttribArray(ATTRIB_ID(COLOR));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void gfx::SimpleMesh::send_index_data(const uint16_t *data,
                                            size_t count) {
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFER_ID(INDEX));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * count, data,
               GL_STATIC_DRAW);
  m_index_count = count;
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gfx::SimpleMesh::draw() {
  // glFrontFace(GL_CW);
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, (void *)0);
  glBindVertexArray(0);
}

#undef ATTRIB_ID
#undef BUFFER_ID
#undef ITER_LOG
#undef EXPR_LOG
