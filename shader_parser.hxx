#ifndef SHADER_PREPROC_HXX
#define SHADER_PREPROC_HXX
#include <istream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

// NOTE: We will let the preprocessor to extract comments.

struct MacroDefinition {
  std::string text;
};

struct FuncLikeMacroDef {
  enum Component {
    FLM_COMP_SEGMENT,
    FLM_COMP_ARG,
    FLM_COMP_ARG_STR,
    FLM_COMP_ARG_STR_CONCAT_LEFT,
    FLM_COMP_ARG_STR_CONCAT_RIGHT
  };
  std::vector<std::string> segments;
  std::vector<FuncLikeMacroDef::Component> components;
};

using ShaderPreprocToken = std::variant<MacroDefinition, FuncLikeMacroDef>;

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
  LEX_TOK_LITERAL,
  LEX_TOK_COMMENT,
  LEX_TOK_PREPROC_DIRECTIVE,
};

enum ShaderLexerState {
  LEX_STATE_PREPROC_DIRECTIVE,
  LEX_STATE_READING_SPACE,
  LEX_STATE_READING_IDENTIFIER,
  LEX_STATE_READING_NUMERIC_LITERAL,
  LEX_STATE_READING_STRING_LITERAL,
  LEX_STATE_MAYBE_PREPROC_DIRECTIVE,
  LEX_STATE_READING_PREPROC_DIRECTIVE,
  LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE,
  LEX_STATE_READING_SINGLE_LINE_COMMENT, // put comment related things after
  LEX_STATE_EXPECTING_COMMENT_OR_DIVISION_OP,
  LEX_STATE_READING_MULTI_LINE_COMMENT,
  LEX_STATE_MAYBE_NESTED_MULTI_LINE_COMMENT,
  LEX_STATE_MAYBE_END_OF_MULTI_LINE_COMMENT, // put comment related things
                                             // before
};

struct ShaderLexerToken {
  size_t begin;
  size_t end;
  ShaderTokenType type;
};

std::ostream &operator<<(std::ostream &strm, const ShaderLexerToken &);

const size_t SHADER_MAX_KEYWORD_LEN = 64;

class ShaderPreprocParser {
public:
  ShaderPreprocParser() = default;
  ShaderPreprocParser(const ShaderPreprocParser &other) = delete;
  ShaderPreprocParser(ShaderPreprocParser &&other) = delete;
  ShaderPreprocParser &operator=(const ShaderPreprocParser &other) = delete;

  void feed(char c);
  void feed(const char *str);

  std::optional<ShaderPreprocToken> finalize();

private:
  enum State {
    SPP_STATE_BEGINNING,
    SPP_STATE_READING_DIRECTIVE_NAME,
    SPP_STATE_READING_MACRO_NAME_DEF,
    SPP_STATE_READING_MACRO_NAME_DEF_BEGINNING,
    SPP_STATE_READING_MACRO_ARG_BEFORE,
    SPP_STATE_READING_MACRO_ARG,
    SPP_STATE_READING_MACRO_ARG_AFTER,
    SPP_STATE_READING_MACRO_DEF,
  };

  struct {
    State state;
    size_t position = 0;
    std::string tmp_str = "";
    std::string macro_name = "";
    std::vector<std::string> macro_args = {};
    ShaderPreprocToken token;
  } m_context;
};

class ShaderLexer {
public:
  ShaderLexer() = default;
  ShaderLexer(const ShaderLexer &&) = delete;
  ShaderLexer(ShaderLexer &&) = delete;
  ShaderLexer &operator=(const ShaderLexer &) = delete;

  void feed(char c);
  void feed(const char *str);
  void feed(const char *str, size_t count);
  // FIXME: finalize should clear context
  void finalize();

private:
  std::vector<ShaderLexerToken> m_tokens;
  struct {
    ShaderLexerState state = LEX_STATE_MAYBE_PREPROC_DIRECTIVE;
    ShaderLexerToken current_token = {0, 0, LEX_TOK_SPACE};
    size_t position = 0;
    std::string keyword_check_str = "";
    int multi_line_comment_depth = 0;
    ShaderPreprocParser preproc_parser;
    std::vector<MacroDefinition> macros;
    std::vector<FuncLikeMacroDef> func_macros;
  } m_context;
  friend class ShaderParserParser;
  friend std::ostream &operator<<(std::ostream &strm, const ShaderLexer &);
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

  friend std::ostream &operator<<(std::ostream &strm,
                                  const ShaderParser &parser);
};

std::ostream &operator<<(std::ostream &strm, const ShaderParser &parser);

class ShaderPreprocError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

#endif // SHADER_PREPROC_HXX
