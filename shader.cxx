#include "gfx.hxx"
#include "iterator.hxx"
#include "utility.hxx"

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

GLuint Shader::id() const { return m_id; }

Shader::Shader() : m_id(0), m_ref_count(new int(1)) {}

Shader Shader::from_file(GLuint type, const std::string &path) {
  auto shader = Shader();

  auto shader_text = read_text_file(path).value_or("");
  if (shader_text.size() == 0) {
    throw std::runtime_error(std::string("Missing shader source code of type ") +
                             stringify_shader_type(type) + "source!");
  }

  shader.m_id = compile_shader(type, shader_text, path);

  return shader;
}

Shader Shader::from_source(GLuint type, const std::string &source) {
  auto shader = Shader();

  shader.m_id = compile_shader(type, source, "<buffer>");

  return shader;
}

Shader::~Shader() {
  *m_ref_count -= 1;
  if (*m_ref_count < 0) {
    std::cerr << "Ref count underflow in Shader class!\n";
  }
  if (*m_ref_count == 0) {
    std::cout << "Deleting shader!\n";
    delete m_ref_count;
    glDeleteShader(m_id);
  }
}

void Shader::swap(Shader &rhs, Shader &lhs) {
  std::swap(rhs.m_id, lhs.m_id);
  std::swap(rhs.m_ref_count, lhs.m_ref_count);
}

Shader::Shader(const Shader &other)
    : m_id(other.m_id), m_ref_count(other.m_ref_count) {
    *m_ref_count += 1;
}
Shader &Shader::operator=(const Shader &other) {
  auto shader = Shader(other);
  std::swap(shader, *this);
  return *this;
}

GLuint ShaderProgram::id() const { return m_id; }

ShaderProgram::ShaderProgram() : m_id(0), m_ref_count(new int(1)) {}

void ShaderProgram::swap(ShaderProgram &rhs, ShaderProgram &lhs) {
  std::swap(rhs.m_id, lhs.m_id);
  std::swap(rhs.m_ref_count, lhs.m_ref_count);
}

ShaderProgram::~ShaderProgram() {
  *m_ref_count -= 1;
  if (*m_ref_count < 0) {
    std::cerr << "Ref count underflow in ShaderProgram class!\n";
  }
  if (*m_ref_count == 0) {
    std::cout << "Deleting shader program!\n";
    delete m_ref_count;
    glDeleteProgram(m_id);
  }
}

ShaderProgram::ShaderProgram(const ShaderProgram &other)
    : m_id(other.m_id), m_ref_count(other.m_ref_count) {
    *m_ref_count += 1;
}
ShaderProgram &ShaderProgram::operator=(const ShaderProgram &other) {
  auto program = ShaderProgram(other);
  std::swap(program, *this);
  return *this;
}

/**
 * Compiles a shader of the specified type from the given source code.
 * @param shader_type OpenGL shader type (e.g., GL_VERTEX_SHADER).
 * @param source Shader source code as a string.
 * @param filename Name of the source file for error reporting.
 * @throws ShaderCompileError on compilation failure.
 * @return GLuint ID of the compiled shader.
 */
GLuint compile_shader(GLenum shader_type, const std::string &source,
                      const std::string &filename) {
  GLuint shader = glCreateShader(shader_type);

  if (shader == 0) {
    throw ShaderCompileError(
        std::string("Failed to create shader object of type ") +
        stringify_shader_type(shader_type) + "(FATAL ERROR)");
  }

  const char *source_cstr = source.c_str();
  GLint length = static_cast<GLint>(source.size());
  glShaderSource(shader, 1, &source_cstr, &length);
  glCompileShader(shader);

  // Check compilation status
  GLint success = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    std::array<char, 1024> log;
    std::fill(begin(log), end(log), 0);
    glGetShaderInfoLog(shader, log.size(), NULL, &log[0]);
    throw ShaderCompileError(filename + ": " + &log[0]);
  }

  return shader;
}

void ShaderProgram::use() const {
    if(m_id == 0) {
        throw std::runtime_error("Tried to use invalid shader program!\n");
    }
    glUseProgram(m_id);
}
