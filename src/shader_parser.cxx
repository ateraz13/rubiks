#include <rubiks/shader_parser.hxx>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

ShaderParser::ShaderParser() : m_ast() {}

void ShaderParser::parse_file(const std::string &filename) {

    std::ifstream file(filename);

    if (!file.is_open() && file.good()) {
        throw std::runtime_error("Failed to open file(Shader Parser)");
    }

    std::array<char, 513> rbuf{0};
    size_t rcount = 0;

    while (!file.eof() && file.good()) {
        // Don't overwrite the whole buffer to keep a null byte at the end.
        file.read(&rbuf[0], rbuf.size() - 1);
        rcount = file.gcount();

        m_lexer.feed(static_cast<const char *>(&rbuf[0]));
    }

    m_lexer.finalize();
}

void ShaderParser::attribute_definition_parsed(
    const AttributeDefinition &ad) const {}

void ShaderParser::uniform_definition_parsed(
    const UniformDefinition &ad) const {}

// FuncLikeMacroDef::FuncLikeMacroDef(const FuncLikeMacroDef& other):
//     segments(other.segments.begin(), other.segments.end()),
//     components(other.components.begin(), other.components.end()),
//     name(other.name){
// }

FuncLikeMacroDef &FuncLikeMacroDef::operator=(const FuncLikeMacroDef &other) {
    auto tmp = FuncLikeMacroDef(other);
    std::swap(*this, tmp);
    return *this;
}

void swap(FuncLikeMacroDef &a, FuncLikeMacroDef &b) {
    std::swap(a.segments, b.segments);
    std::swap(a.components, b.components);
    std::swap(a.name, b.name);
}

std::ostream &operator<<(std::ostream &strm, const ShaderLexerToken &token) {
    strm << "{ begin = " << token.seg.begin << ", end = " << token.seg.end
         << ", type = ";
    switch (token.type) {
    case LEX_TOK_SPACE:
        strm << "Space";
        break;
    case LEX_TOK_KEYWORD:
        strm << "Keyword";
        break;
    case LEX_TOK_IDENTIFIER:
        strm << "Identifier";
        break;
    case LEX_TOK_OPERATOR:
        strm << "Operator";
        break;
    case LEX_TOK_PUNCTUATION:
        strm << "Punctuation";
        break;
    case LEX_TOK_LITERAL:
        strm << "Literal";
        break;
    case LEX_TOK_COMMENT:
        strm << "Comment";
        break;
    default:
        strm << "Unknown token";
        break;
    }
    strm << "}";
    return strm;
}

std::ostream &operator<<(std::ostream &strm, const ShaderLexer &lexer) {

    strm << "Lexer { \n";
    for (auto tok : lexer.m_tokens) {
        strm << tok << "\n";
    }
    strm << "}";
    return strm;
}

bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n'; }

bool is_digit(char c) { return c >= '0' && c <= '9'; }

bool is_alphabetic(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// TODO: Create a table of hashes for all the keywords
// match identifiers against the hashes to change them to keywords.
//
// NOTE: We can store identifiers as hashes

bool is_keyword(const std::string &str) {

    static bool initialized = false;
    static std::array<std::string, 11> keywords = {
        "volatile", "const",     "if",      "else",   "switch", "for",
        "struct",   "attribute", "uniform", "return", "case"};
    static std::array<decltype(std::hash<std::string>{}(std::string())),
                      keywords.size()>
        keyword_hashes;

    if (!initialized) {
        auto add_keyword = [&](std::string kw) {
            static int i = 0;
            if (i >= keywords.size()) {
                return;
            }
            keyword_hashes[i] = std::hash<std::string>{}(kw);
            i++;
        };

        for (auto kw : keywords) {
            add_keyword(kw);
        }

        std::sort(keyword_hashes.begin(), keyword_hashes.end());

        auto prev = keyword_hashes.begin(), end = keyword_hashes.end(),
             iter = prev++;

        while (iter != end) {
            if (*prev == *iter) {
                throw std::runtime_error(
                    "Hash collision detected within keyword hashes used by "
                    "shader parser!\n");
            }
            iter++;
            prev++;
        }

        initialized = true;
    }

    auto h = std::hash<std::string>{}(str);

    return std::binary_search(keyword_hashes.begin(), keyword_hashes.end(), h);
}

bool is_punctuation(char c) {
    switch (c) {
    case '.':
    case '{':
    case '}':
    case '[':
    case ']':
    case '(':
    case ')':
    case ';':
    case ':':
        return true;
        break;
    default:
        return false;
    }
}

bool is_operator(char c) {
    switch (c) {
    case '+':
    case '-':
    case '/':
    case '*':
    case '>':
    case '<':
    case '|':
    case '&':
    case '^':
    case '%':
    case '!':
    case '=':
    case '?':
        return true;
        break;
    default:
        return false;
    }
}

std::optional<ShaderPreprocToken> ShaderPreprocParser::finalize() {
    switch (m_context.state) {
    case SPP_STATE_READING_MACRO_DEF:
    case SPP_STATE_READING_MACRO_DEF_IDENTIFIER:
        m_context.current_segment.end = m_context.position;
        if (auto *fm = std::get_if<FuncLikeMacroDef>(&m_context.token)) {
            fm->segments.push_back(m_context.current_segment);
        } else {
            std::get<MacroDefinition>(m_context.token).seg =
                m_context.current_segment;
        }
        break;
    }
    if (auto *fm = std::get_if<FuncLikeMacroDef>(&m_context.token)) {
        switch (m_context.state) {
        case SPP_STATE_READING_MACRO_DEF_IDENTIFIER:
            if (auto it =
                    std::find(m_context.macro_args.begin(),
                              m_context.macro_args.end(), m_context.tmp_str);
                it != m_context.macro_args.end()) {
                // FIXME: Index the argument please.
                fm->components.push_back(FLM_COMP_ARG);
            }
            break;
        default:
            fm->components.push_back(FLM_COMP_SEGMENT);
        }
    }

    ShaderPreprocToken token(m_context.token);
    m_context = Context();

    return token;
}

void ShaderLexer::feed(char c) {

    auto start_new_tok = [&](ShaderTokenType type) {
        std::cout << "m_context.position = " << m_context.position << "\n";
        m_context.current_token.seg.begin = m_context.position;
        m_context.current_token.seg.end = m_context.position + 1;
        m_context.current_token.type = type;
    };

    auto finalize_identifier = [&]() {
        if (is_keyword(m_context.keyword_check_str)) {
            std::cout << "Is keyword!\n";
            m_context.current_token.type = LEX_TOK_KEYWORD;
        };
        m_tokens.push_back(m_context.current_token);
        m_context.current_token = ShaderLexerToken();
        m_context.keyword_check_str.clear();
    };

    bool not_within_string_literal =
        m_context.state != LEX_STATE_READING_STRING_LITERAL &&
        m_context.state != LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE;

    bool not_within_comment =
        !(m_context.state >= LEX_STATE_READING_SINGLE_LINE_COMMENT &&
          m_context.state <= LEX_STATE_MAYBE_END_OF_MULTI_LINE_COMMENT);

    bool not_within_macro_def =
        m_context.state != LEX_STATE_MAYBE_PREPROC_DIRECTIVE &&
        m_context.state != LEX_STATE_READING_PREPROC_DIRECTIVE;

    bool within_normal_state =
        not_within_string_literal && not_within_comment && not_within_macro_def;

    if (m_context.state == LEX_STATE_MAYBE_PREPROC_DIRECTIVE) {
        if (c == '#') {
            start_new_tok(LEX_TOK_PREPROC_DIRECTIVE);
            m_context.state = LEX_STATE_READING_PREPROC_DIRECTIVE;
        } else {
            m_context.state = LEX_STATE_READING_SPACE;
            m_context.current_token = ShaderLexerToken();
            m_context.current_token.seg.begin = m_context.position;
            m_context.current_token.seg.end = m_context.position;
            this->feed(c);
            return;
        }
    }

    if (is_space(c) && m_context.state != LEX_STATE_READING_SPACE &&
        within_normal_state) {
        if (m_context.state == LEX_STATE_READING_IDENTIFIER) {
            finalize_identifier();
        } else {
            m_tokens.push_back(m_context.current_token);
        }
        start_new_tok(LEX_TOK_SPACE);
        m_context.state = LEX_STATE_READING_SPACE;
    }

    if (is_space(c) && within_normal_state) {
        if (c == '\n') {
            m_context.state = LEX_STATE_MAYBE_PREPROC_DIRECTIVE;
        }
        m_context.position++;
        return;
    }

    switch (m_context.state) {
    case LEX_STATE_READING_PREPROC_DIRECTIVE:
        // FIXME: implement newline escapes
        if (c == '\n') {
            if (auto maybe_val = m_context.preproc_parser.finalize()) {
                auto val = *maybe_val;

                if (std::holds_alternative<MacroDefinition>(val)) {
                    m_context.macros.push_back(std::get<MacroDefinition>(val));
                } else if (std::holds_alternative<FuncLikeMacroDef>(val)) {
                    m_context.func_macros.push_back(
                        std::get<FuncLikeMacroDef>(val));
                } else {
                    throw std::runtime_error(
                        "Preprocessor directive not implemented!");
                }
            }
            m_context.state = LEX_STATE_MAYBE_PREPROC_DIRECTIVE;
        } else {
            m_context.preproc_parser.feed(c);
        }
        break;
    case LEX_STATE_READING_SPACE:
        if (is_digit(c) || c == '"') {
            start_new_tok(LEX_TOK_LITERAL);

            if (c == '"') {
                m_context.state = LEX_STATE_READING_STRING_LITERAL;
            } else {
                m_context.state = LEX_STATE_READING_NUMERIC_LITERAL;
            }

        } else if (is_alphabetic(c)) {
            start_new_tok(LEX_TOK_IDENTIFIER);
            m_context.state = LEX_STATE_READING_IDENTIFIER;
            this->feed(c);
            return;
            // m_context.keyword_check_str.push_back(c);
        } else if (is_punctuation(c)) {
            start_new_tok(LEX_TOK_PUNCTUATION);
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderLexerToken();
        } else if (is_operator(c)) {
            start_new_tok(LEX_TOK_OPERATOR);
            if (c == '/') {
                m_context.state = LEX_STATE_EXPECTING_COMMENT_OR_DIVISION_OP;
            } else {
                m_tokens.push_back(m_context.current_token);
                m_context.current_token = ShaderLexerToken();
            }
        }
        break;
    case LEX_STATE_EXPECTING_COMMENT_OR_DIVISION_OP:
        if (c == '/') {
            m_context.state = LEX_STATE_READING_SINGLE_LINE_COMMENT;
            m_context.current_token.type = LEX_TOK_COMMENT;
            m_context.current_token.seg.end++;
        } else if (c == '*') {
            m_context.state = LEX_STATE_READING_MULTI_LINE_COMMENT;
            m_context.multi_line_comment_depth += 1;
            m_context.current_token.type = LEX_TOK_COMMENT;
            m_context.current_token.seg.end++;
        } else {
            m_tokens.push_back(m_context.current_token);
            m_context.state = LEX_STATE_READING_SPACE;
            m_context.current_token = ShaderLexerToken();
            this->feed(c);
            return;
        }
        break;
    case LEX_STATE_READING_SINGLE_LINE_COMMENT:
        if (c != '\n') {
            m_context.current_token.seg.end++;
        } else {
            m_tokens.push_back(m_context.current_token);
            m_context.state = LEX_STATE_READING_SPACE;
            start_new_tok(LEX_TOK_SPACE);
        }
        break;
    case LEX_STATE_READING_MULTI_LINE_COMMENT:
        m_context.current_token.seg.end++;
        if (c == '/') {
            m_context.state = LEX_STATE_MAYBE_NESTED_MULTI_LINE_COMMENT;
        }
        if (c == '*') {
            m_context.state = LEX_STATE_MAYBE_END_OF_MULTI_LINE_COMMENT;
        }
        break;
    case LEX_STATE_MAYBE_NESTED_MULTI_LINE_COMMENT:
        if (c == '*') {
            m_context.multi_line_comment_depth += 1;
        }
        m_context.current_token.seg.end++;
        m_context.state = LEX_STATE_READING_MULTI_LINE_COMMENT;
        break;
    case LEX_STATE_MAYBE_END_OF_MULTI_LINE_COMMENT:
        if (c == '/') {
            m_context.multi_line_comment_depth -= 1;
            if (m_context.multi_line_comment_depth == 0) {
                m_context.current_token.seg.end++;
                m_tokens.push_back(m_context.current_token);
                m_context.current_token = ShaderLexerToken();
                m_context.current_token.seg.begin = m_context.position;
                m_context.current_token.seg.end = m_context.position;
                m_context.state = LEX_STATE_READING_SPACE;
            }
        } else {
            m_context.state = LEX_STATE_READING_MULTI_LINE_COMMENT;
            this->feed(c);
            return;
        }
        break;
    case LEX_STATE_READING_STRING_LITERAL:
        m_context.current_token.seg.end++;
        if (c == '"') {
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderLexerToken();
            m_context.current_token.seg.begin = m_context.position;
            m_context.current_token.seg.end = m_context.position;
            m_context.current_token.type = LEX_TOK_SPACE;
            m_context.state = LEX_STATE_READING_SPACE;
        }
        if (c == '\\') {
            m_context.state = LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE;
        }
        break;
    case LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE:
        m_context.current_token.seg.end++;
        m_context.state = LEX_STATE_READING_STRING_LITERAL;
        break;
    case LEX_STATE_READING_NUMERIC_LITERAL:
        if (!is_digit(c)) {
            m_tokens.push_back(m_context.current_token);
            m_context.state = LEX_STATE_READING_SPACE;
            this->feed(c);
            return;
        }
        m_context.current_token.seg.end++;
        break;
    case LEX_STATE_READING_IDENTIFIER:
        if (!(is_alphabetic(c) || is_digit(c) || c == '_')) {
            finalize_identifier();
            m_context.current_token.seg.begin = m_context.position;
            m_context.current_token.seg.end = m_context.position;
            m_context.current_token.type = LEX_TOK_SPACE;
            m_context.state = LEX_STATE_READING_SPACE;
            this->feed(c);
            return;
        } else {
            m_context.keyword_check_str.push_back(c);
            m_context.current_token.seg.end++;
        }
        break;

    default:
        throw ShaderPreprocError("Invalid state in Shader Preprocessor.");
        break;
    }
    m_context.position++;
    // FIXME: Currently when there is unidentified token the lexer just
    // ignores it.
}

void ShaderLexer::feed(const char *str) {
    size_t i = 0;
    while (str[i] != '\0') {
        feed(str[i]);
        i++;
    }
}

void after_finalize() { return; }

void ShaderLexer::finalize() {
    switch (m_context.state) {
    case LEX_STATE_READING_SPACE:
    case LEX_STATE_MAYBE_PREPROC_DIRECTIVE:
        m_tokens.push_back(m_context.current_token);
        m_context.current_token = ShaderLexerToken();
        m_context.multi_line_comment_depth = 0;
        m_context.keyword_check_str = "";
        break;
    default:
        break;
    }

    after_finalize();
}

std::ostream &operator<<(std::ostream &strm, const ShaderParser &parser) {
    strm << parser.m_lexer << std::endl;
    return strm;
}

void ShaderPreprocParser::feed(char c) {
    switch (m_context.state) {
    case SPP_STATE_BEGINNING:
        if (c != '#') {
            throw ShaderPreprocError("Expected a # symbol");
        }
        m_context.state = SPP_STATE_BEFORE_DIRECTIVE_NAME;
        break;
    case SPP_STATE_BEFORE_DIRECTIVE_NAME:
        if (is_space(c)) {
            m_context.position++;
            return;
        }
        if (is_alphabetic(c) || is_digit(c)) {
            m_context.state = SPP_STATE_READING_DIRECTIVE_NAME;
            this->feed(c);
            return;
        } else {
            throw ShaderPreprocError(
                "Invalid token, expecting an space, alphabetic "
                "character or an underscore!");
        }
        break;
    case SPP_STATE_READING_DIRECTIVE_NAME:
        if (is_space(c)) {
            if (m_context.tmp_str == "define") {
                m_context.state = SPP_STATE_READING_MACRO_NAME_DEF;
                m_context.tmp_str.clear();
                break;
            }
            m_context.tmp_str.clear();
        }
        if (is_alphabetic(c) || is_digit(c) || c == '_') {
            m_context.tmp_str.push_back(c);
        } else {
            throw ShaderPreprocError("Invalid token, expecting an alphabetic, "
                                     "numeric characters or an underscore!");
        }
        break;
    case SPP_STATE_READING_MACRO_NAME_DEF_BEGINNING:
        if (is_space(c)) {
            m_context.position++;
            return;
        }
        if (is_alphabetic(c) || is_digit(c)) {
            m_context.macro_name.push_back(c);
        } else {
            throw ShaderPreprocError("Invalid token, expecting an alphabetic "
                                     "character or an underscore!");
        }
        break;
    case SPP_STATE_READING_MACRO_NAME_DEF:
        if (c == '(') {
            auto fm = FuncLikeMacroDef();
            fm.name = m_context.tmp_str;
            std::cout << "tmp_str (fm name def) = \"" << m_context.tmp_str
                      << "\"\n";
            m_context.tmp_str.clear();
            m_context.token = fm;
            m_context.state = SPP_STATE_READING_MACRO_ARG;
            break;
        } else if (is_space(c)) {
            m_context.position++;
            m_context.state = SPP_STATE_READING_MACRO_DEF;
            auto macro = MacroDefinition();
            macro.name = m_context.tmp_str;
            m_context.tmp_str.clear();
            macro.seg.begin = m_context.position;
            macro.seg.end = m_context.position;
            m_context.token = macro;
            m_context.current_segment.begin = m_context.position;
            m_context.current_segment.end = m_context.position;
            return;
        } else if (is_alphabetic(c) || is_digit(c) || c == '_') {
            m_context.tmp_str.push_back(c);
        } else {
            throw ShaderPreprocError(
                "Invalid token, expecting an alphabetic, numeric characters "
                "or an underscore!");
        }
        break;
    case SPP_STATE_READING_MACRO_ARG_BEFORE:
        if (is_alphabetic(c) || c == '_') {
            m_context.tmp_str.clear();
            m_context.tmp_str.push_back(c);
            m_context.state = SPP_STATE_READING_MACRO_ARG;
        } else if (!is_space(c)) {
            throw ShaderPreprocError("Invalid character, expected alphabetic "
                                     "character, empty space, ',' or ')'!");
        }
        break;
    case SPP_STATE_READING_MACRO_ARG:
        // FIXME: newlines could be escaped and treated as spaces or it can be
        // done outside of the parser.
        if (c == ',') {
            m_context.macro_args.push_back(m_context.tmp_str);
            m_context.tmp_str.clear();
            m_context.state = SPP_STATE_READING_MACRO_ARG_BEFORE;
        } else if (is_alphabetic(c) || is_digit(c) || c == '_') {
            m_context.tmp_str.push_back(c);
        } else {
            m_context.state = SPP_STATE_READING_MACRO_ARG_AFTER;
            this->feed(c);
            return;
        }
        // } else {
        //     throw ShaderPreprocError("Invalid character, expected
        //     alphanumeric "
        //                              "character,empty space, ',' or ')'!");
        // }
        break;
    case SPP_STATE_READING_MACRO_ARG_AFTER:
        if (c == ')') {
            m_context.macro_args.push_back(m_context.tmp_str);
            m_context.tmp_str.clear();
            m_context.state = SPP_STATE_READING_MACRO_DEF;
            m_context.current_segment.begin = m_context.position + 1;
            m_context.current_segment.end = m_context.position + 1;
            // We require the arguments to be ordered in the way they appear in
            // the argument list. std::sort(m_context.macro_args.begin(),
            // m_context.macro_args.end());
        } else if (c == ',') {
            m_context.macro_args.push_back(m_context.tmp_str);
            m_context.tmp_str.clear();
            m_context.state = SPP_STATE_READING_MACRO_ARG_BEFORE;
        } else if (!is_space(c)) {
            throw ShaderPreprocError(
                "Invalid character, expected empty space, ',' or ')'!");
        }
        break;
    case SPP_STATE_READING_MACRO_DEF:
        if (auto *macro = std::get_if<MacroDefinition>(&m_context.token)) {
            macro->seg.end++;
        } else if (auto *func_macro =
                       std::get_if<FuncLikeMacroDef>(&m_context.token)) {
            if (is_alphabetic(c) || c == '_') {
                m_context.state = SPP_STATE_READING_MACRO_DEF_IDENTIFIER;
                m_context.current_segment.end = m_context.position;
                func_macro->components.push_back(FLM_COMP_SEGMENT);
                func_macro->segments.push_back(m_context.current_segment);
                m_context.current_segment.begin = m_context.position;
                m_context.tmp_str.clear();
                this->feed(c);
                return;
            } else if (c == '#') {
                func_macro->segments.push_back(m_context.current_segment);
                func_macro->components.push_back(FLM_COMP_SEGMENT);
                func_macro->components.push_back(FLM_COMP_ARG_STR);
                m_context.state = SPP_STATE_READING_MACRO_DEF_HASH;
            }
        } else {
            throw ShaderPreprocError(
                "Invalid macro definition, there is a bug in the software!");
        }
        break;
    case SPP_STATE_READING_MACRO_DEF_HASH: {
        auto func_macro = std::get<FuncLikeMacroDef>(m_context.token);
        if (c == '#') {
            func_macro.components[func_macro.components.size() - 1] =
                FLM_COMP_ARG_CONCAT;
            m_context.state = SPP_STATE_READING_MACRO_DEF;
        } else if (is_alphabetic(c) || is_digit(c) || c == '_') {
            m_context.state = SPP_STATE_READING_MACRO_DEF_IDENTIFIER;
            m_context.tmp_str.clear();
            m_context.current_segment.begin = m_context.position;
            m_context.current_segment.end = m_context.position + 1;
        } else {
            throw ShaderPreprocError("Expected an macro paramenter after '#'");
        }
    } break;
    case SPP_STATE_READING_MACRO_DEF_IDENTIFIER: {
        auto func_macro = std::get<FuncLikeMacroDef>(m_context.token);
        if (is_alphabetic(c) || is_digit(c) || c == '_') {
            m_context.current_segment.end++;
            m_context.tmp_str.push_back(c);
        } else {
            m_context.state = SPP_STATE_READING_MACRO_DEF;
            m_context.current_segment.end = m_context.position;
            auto *fm = std::get_if<FuncLikeMacroDef>(&m_context.token);
            assert(fm != nullptr);
            auto it = std::find(m_context.macro_args.begin(),
                                m_context.macro_args.end(), m_context.tmp_str);
            if (it != m_context.macro_args.end()) {
                // FIXME: We need to actually specify the index of the argument.
                fm->components.push_back(FLM_COMP_ARG);
            } else {
                fm->components.push_back(FLM_COMP_SEGMENT);
            }
            fm->segments.push_back(m_context.current_segment);
            m_context.current_segment.begin = m_context.position;
            this->feed(c);
            return;
        }
    } break;
    }
    m_context.position++;
}

void ShaderPreprocParser::feed(const char *str) {
    while (*str != '\0') {
        feed(*str);
        str++;
    }
}

void ShaderPreprocParser::feed(const char *str, size_t count) {
    while (*str != '\0' && count) {
        feed(*str);
        count--;
        str++;
    }
}

// int main(int argc, char **argv) {

//     if (argc < 3) {
//         std::cerr << "Specify input and output files" << std::endl;
//         return 1;
//     }

//     std::cout << "Hello, world!\n";

//     std::cout << "Output file: " << argv[2] << std::endl;
//     std::cout << "Input file: " << argv[1] << std::endl;
//     std::ofstream out_file;

//     out_file.open(argv[2], std::ofstream::out);

//     if (!out_file.is_open()) {
//         std::cout << "Could not open output file\n";
//         return 1;
//     }

//     ShaderParser parser;
//     parser.parse_file(argv[1]);
//     out_file << parser << std::endl;
//     out_file.close();
// }
