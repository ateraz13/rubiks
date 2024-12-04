#ifndef SHADER_HXX
#define SHADER_HXX
#include "gfx.hxx"
#include "gl.hxx"
#include <string>
#include <stdexcept>
#include <array>
#include <iostream>
#include <algorithm>

class ShaderCompileError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class ShaderProgramLinkingError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

std::string stringify_shader_type(GLenum shader_type);

class Shader {
public:
  GLuint id() const;
  static Shader from_file(GLuint type, const std::string &path);
  static Shader from_source(GLuint type, const std::string &source);

  void swap(Shader &rhs, Shader &lfs);

  Shader(const Shader &other);
  Shader &operator=(const Shader &other);
  ~Shader();

private:
  Shader();
  GLuint m_id;
  int* m_ref_count;
};

class ShaderProgram {
public:
  GLuint id() const;
  void use() const;

  template <typename Iterator>
  static ShaderProgram link(Iterator begin, Iterator end);

  void swap(ShaderProgram &rhs, ShaderProgram &lfs);

  ShaderProgram(const ShaderProgram &other);
  ShaderProgram &operator=(const ShaderProgram &other);
  ~ShaderProgram();

private:
  ShaderProgram();
  GLuint m_id;
  int* m_ref_count;
};

static GLuint compile_shader(GLenum shader_type, const std::string &source,
                             const std::string &filename);
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
GLuint link_shader_program(Iterator begin, Iterator end) {
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
    std::cout << "attaching a shader " << shader_id << std::endl;
    glAttachShader(shader_program, shader_id);
  });

  glLinkProgram(shader_program);

  GLint success = GL_FALSE;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

  if (success == GL_FALSE) {
    glGetProgramInfoLog(shader_program, 1024, NULL, &log[0]);
    throw ShaderProgramLinkingError(&log[0]);
  }

  std::for_each(begin, end, [=](GLuint shader_id) {
    glDetachShader(shader_program, shader_id);
  });

  return shader_program;
}

template <typename Iterator>
ShaderProgram ShaderProgram::link(Iterator begin, Iterator end) {
  static_assert(
      std::is_same_v<typename std::iterator_traits<Iterator>::value_type,
                     Shader>,
      "The link builder function takes in an iterator over Shader objects.");

  auto program = ShaderProgram();
  auto extract_id = [](Shader &shader) { return shader.id(); };
  auto mapped_begin = make_mapping_iterator(begin, extract_id);
  auto mapped_end = make_mapping_iterator(end, extract_id);

  program.m_id = link_shader_program(mapped_begin, mapped_end);

  return program;
}

#endif // SHADER_HXX
