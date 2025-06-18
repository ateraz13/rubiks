#include "shader_parser.hxx"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>

ShaderParser::ShaderParser() : m_ast() {}

void ShaderParser::parse_file(const std::string &filename) {

    std::ifstream file(filename);

    if (!file.is_open() && file.good()) {
        throw std::runtime_error("Failed to open file(Shader Parseressor)");
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

std::ostream &operator<<(std::ostream &strm, const ShaderLexerToken &token) {
    strm << "{ begin = " << token.begin << ", end = " << token.end
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

ShaderLexer::ShaderLexer() : m_tokens(), m_context() {}

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

void ShaderLexer::feed(char c) {

    auto start_new_tok = [&](ShaderTokenType type) {
        std::cout << "m_context.position = " << m_context.position << "\n";
        m_context.current_token.begin = m_context.position;
        m_context.current_token.end = m_context.position + 1;
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

    bool within_normal_state = not_within_string_literal && not_within_comment;

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
        m_context.position++;
        return;
    }

    switch (m_context.state) {
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
                m_context.current_token.type = LEX_TOK_COMMENT;
            } else {
                m_tokens.push_back(m_context.current_token);
                m_context.current_token = ShaderLexerToken();
            }
        }
        break;
    case LEX_STATE_EXPECTING_COMMENT_OR_DIVISION_OP:
        // FIXME: Comments can be stacked, comments within comments
        if (c == '/') {
            m_context.state = LEX_STATE_READING_SINGLE_LINE_COMMENT;
            m_context.current_token.end++;
        } else if (c == '*') {
            m_context.state = LEX_STATE_READING_MULTI_LINE_COMMENT;
            m_context.current_token.type = LEX_TOK_COMMENT;
            m_context.current_token.end++;
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
            m_context.current_token.end++;
        } else {
            m_tokens.push_back(m_context.current_token);
            m_context.state = LEX_STATE_READING_SPACE;
            start_new_tok(LEX_TOK_SPACE);
        }
        break;
    case LEX_STATE_READING_MULTI_LINE_COMMENT:
        if (c == '*') {
            m_context.current_token.end++;
            m_context.state = LEX_STATE_MAYBE_END_OF_MULTI_LINE_COMMENT;
        } else {
            m_context.current_token.end++;
        }
        break;
    case LEX_STATE_MAYBE_END_OF_MULTI_LINE_COMMENT:
        if (c == '/') {
            m_context.current_token.end++;
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderLexerToken();
            m_context.current_token.begin = m_context.position;
            m_context.current_token.end = m_context.position;
            m_context.state = LEX_STATE_READING_SPACE;
        } else {
            m_context.state = LEX_STATE_READING_MULTI_LINE_COMMENT;
            this->feed(c);
            return;
        }
        break;
    case LEX_STATE_READING_STRING_LITERAL:
        m_context.current_token.end++;
        if (c == '"') {
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderLexerToken();
            m_context.current_token.begin = m_context.position;
            m_context.current_token.end = m_context.position;
            m_context.current_token.type = LEX_TOK_SPACE;
            m_context.state = LEX_STATE_READING_SPACE;
        }
        if (c == '\\') {
            m_context.state = LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE;
        }
        break;
    case LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE:
        m_context.current_token.end++;
        m_context.state = LEX_STATE_READING_STRING_LITERAL;
        break;
    case LEX_STATE_READING_NUMERIC_LITERAL:
        if (!is_digit(c)) {
            m_tokens.push_back(m_context.current_token);
            m_context.state = LEX_STATE_READING_SPACE;
            this->feed(c);
            return;
        }
        m_context.current_token.end++;
        break;
    case LEX_STATE_READING_IDENTIFIER:
        if (!(is_alphabetic(c) || is_digit(c) || c == '_')) {
            finalize_identifier();
            m_context.current_token.begin = m_context.position;
            m_context.current_token.end = m_context.position;
            m_context.current_token.type = LEX_TOK_SPACE;
            m_context.state = LEX_STATE_READING_SPACE;
            this->feed(c);
            return;
        } else {
            m_context.keyword_check_str.push_back(c);
            m_context.current_token.end++;
        }
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

void ShaderLexer::finalize() {
    if (m_context.state != LEX_STATE_READING_SPACE) {
        m_tokens.push_back(m_context.current_token);
        m_context.current_token = ShaderLexerToken();
    }
}

std::ostream &operator<<(std::ostream &strm, const ShaderParser &parser) {
    strm << parser.m_lexer << std::endl;
    return strm;
}

int main(int argc, char **argv) {

    if (argc < 3) {
        std::cerr << "Specify input and output files" << std::endl;
        return 1;
    }

    std::cout << "Hello, world!\n";

    std::cout << "Output file: " << argv[2] << std::endl;
    std::cout << "Input file: " << argv[1] << std::endl;
    std::ofstream out_file;

    out_file.open(argv[2], std::ofstream::out);

    if (!out_file.is_open()) {
        std::cout << "Could not open output file\n";
        return 1;
    }

    ShaderParser parser;
    parser.parse_file(argv[1]);
    out_file << parser << std::endl;
    out_file.close();
}
