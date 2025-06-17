#ifndef SHADER_PREPROC_HXX
#define SHADER_PREPROC_HXX
#include <istream>
#include <string>
#include <type_traits>
#include <string>
#include <vector>

class ShaderParserAst {
  public:
};
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

enum ShaderLexerState {
  LEX_STATE_READING_SPACE,
  LEX_STATE_READING_IDENTIFIER,
  LEX_STATE_READING_NUMERIC_LITERAL,
  LEX_STATE_READING_STRING_LITERAL,
  LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE,
};

struct ShaderLexerToken {
  size_t begin;
  size_t end;
  ShaderTokenType type;
};

std::ostream &operator<<(std::ostream &strm, const ShaderLexerToken &);

const size_t SHADER_MAX_KEYWORD_LEN = 64;

class ShaderLexer {
public:
  ShaderLexer();
  ShaderLexer(const ShaderLexer &&) = delete;
  ShaderLexer(ShaderLexer &&) = delete;
  ShaderLexer &operator=(const ShaderLexer &) = delete;

  void feed(char c);
  void feed(const char *str);
  void finalize();

private:
  std::vector<ShaderLexerToken> m_tokens;
  struct {
    ShaderLexerState state = LEX_STATE_READING_SPACE;
    ShaderLexerToken current_token = {0, 0, LEX_TOK_SPACE};
    size_t position = 0;
    std::string keyword_check_str = "";
  } m_context;
  friend class ShaderParserParser;
  friend std::ostream &operator<<(std::ostream &strm,
                                  const ShaderLexer &);
};

class ShaderParser {
public:
  ShaderParser();
  ShaderParser(const ShaderParser &other) = delete;
  ShaderParser(ShaderParser &&other) = delete;

  ShaderParser &operator=(const ShaderParser &) = delete;

  void parse_file(const std::string &filename);

  virtual void attribute_definition_parsed(const AttributeDefinition &ad) const;
  virtual void uniform_definition_parsed(const UniformDefinition &ud) const;

private:
  ShaderLexer m_lexer;
  ShaderParserAst m_ast;

  friend std::ostream& operator<<(std::ostream& strm, const ShaderParser& parser);
};

std::ostream& operator<<(std::ostream& strm, const ShaderParser& parser);

#endif // SHADER_PREPROC_HXX
