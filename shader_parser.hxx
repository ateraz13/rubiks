#ifndef SHADER_PREPROC_HXX
#define SHADER_PREPROC_HXX
#include <istream>
#include <string>
#include <type_traits>
#include <string>
#include <vector>

class ShaderPreprocAst {};
class AttributeDefinition {};
class UniformDefinition {};
enum ShaderTokenType {
  LEX_TOK_SPACE = 0,
  LEX_TOK_KEYWORD,
  LEX_TOK_IDENTIFIER,
  LEX_TOK_OPERATOR,
  LEX_TOK_PUNCTUATION,
  LEX_TOK_LITERAL
};

enum ShaderPreprocLexerState {
  LEX_STATE_READING_SPACE,
  LEX_STATE_READING_IDENTIFIER,
  LEX_STATE_READING_NUMERIC_LITERAL,
  LEX_STATE_READING_STRING_LITERAL,
  LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE,
};

struct ShaderPreprocLexerToken {
  size_t begin;
  size_t end;
  ShaderTokenType type;
};

std::ostream &operator<<(std::ostream &strm, const ShaderPreprocLexerToken &);

const size_t SHADER_MAX_KEYWORD_LEN = 64;

class ShaderPreprocLexer {
public:
  ShaderPreprocLexer();
  ShaderPreprocLexer(const ShaderPreprocLexer &&) = delete;
  ShaderPreprocLexer(ShaderPreprocLexer &&) = delete;
  ShaderPreprocLexer &operator=(const ShaderPreprocLexer &) = delete;

  void feed(char c);
  void feed(const char *str);
  void finalize();

private:
  std::vector<ShaderPreprocLexerToken> m_tokens;
  struct {
    ShaderPreprocLexerState state = LEX_STATE_READING_SPACE;
    ShaderPreprocLexerToken current_token = {0, 0, LEX_TOK_SPACE};
    size_t position = 0;
    std::string keyword_check_str = "";
  } m_context;
  friend class ShaderPreprocParser;
  friend std::ostream &operator<<(std::ostream &strm,
                                  const ShaderPreprocLexer &);
};

class ShaderPreprocParser {};

class ShaderPreproc {
public:
  ShaderPreproc();
  ShaderPreproc(const ShaderPreproc &other) = delete;
  ShaderPreproc(ShaderPreproc &&other) = delete;

  ShaderPreproc &operator=(const ShaderPreproc &) = delete;

  void parse_file(const std::string &filename);

  virtual void attribute_definition_parsed(const AttributeDefinition &ad) const;
  virtual void uniform_definition_parsed(const UniformDefinition &ud) const;

private:
  ShaderPreprocAst m_ast;
};

#endif // SHADER_PREPROC_HXX
