#include "shader_preproc.hxx"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>

ShaderPreproc::ShaderPreproc() : m_ast() {}

void ShaderPreproc::parse_file(const std::string &filename) {

    std::ifstream file(filename);

    if (!file.is_open() && file.good()) {
        throw std::runtime_error("Failed to open file(Shader Preprocessor)");
    }

    std::array<char, 513> rbuf{0};
    size_t rcount = 0;

    ShaderPreprocLexer lexer;

    while (!file.eof() && file.good()) {
        // Don't overwrite the whole buffer to keep a null byte at the end.
        file.read(&rbuf[0], rbuf.size() - 1);
        rcount = file.gcount();

        lexer.feed(static_cast<const char *>(&rbuf[0]));
    }

    std::cout << lexer << std::endl;

}

void ShaderPreproc::attribute_definition_parsed(
    const AttributeDefinition &ad) const {}

void ShaderPreproc::uniform_definition_parsed(
    const UniformDefinition &ad) const {}

std::ostream &operator<<(std::ostream &strm,
                         const ShaderPreprocLexerToken &token) {
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
    default:
        strm << "Unknown token";
        break;
    }
    strm << "}";
    return strm;
}

std::ostream &operator<<(std::ostream &strm, const ShaderPreprocLexer &lexer) {

    strm << "Lexer { \n";
    for (auto tok : lexer.m_tokens) {
        strm << tok << "\n";
    }
    strm << "}";
    return strm;
}

ShaderPreprocLexer::ShaderPreprocLexer() : m_tokens(), m_context() {}

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
    static std::array<std::string, 8> keywords = {
        "if",     "else",   "switch",    "for",
        "struct", "struct", "attribute", "return"};
    static std::array<decltype(std::hash<std::string>{}(std::string())),
                        keywords.size()>
        keyword_hashes;

    if (!initialized) {
        auto add_keyword = [&](std::string kw) {
            static int i = 0;
            if(i >= keywords.size()) {
                return;
            }
            keyword_hashes[i] = std::hash<std::string>{}(kw);
            i++;
        };

        for(auto kw: keywords) {
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
        return true;
        break;
    default:
        return false;
    }
}

bool is_operator(char c) {
    switch (c) {
    case '.':
    case '{':
    case '}':
    case '[':
    case ']':
        return true;
        break;
    default:
        return false;
    }
}

void ShaderPreprocLexer::feed(char c) {

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
      m_context.current_token = ShaderPreprocLexerToken();
      m_context.keyword_check_str.clear();
    };

    if (is_space(c) && m_context.state != LEX_STATE_READING_SPACE) {
      if (m_context.state == LEX_STATE_READING_IDENTIFIER) {
        finalize_identifier();
      }
      else {
        m_tokens.push_back(m_context.current_token);
      }
      start_new_tok(LEX_TOK_SPACE);
      m_context.state = LEX_STATE_READING_SPACE;
    }

    if (is_space(c)) {
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
            m_context.current_token = ShaderPreprocLexerToken();
        } else if (is_operator(c)) {
            start_new_tok(LEX_TOK_IDENTIFIER);
            m_context.current_token.end++;
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderPreprocLexerToken();
        }
        break;
    case LEX_STATE_READING_STRING_LITERAL:
        m_context.current_token.end++;
        if (c == '"') {
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderPreprocLexerToken();
        }
        if (c == '\\') {
            m_context.state = LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE;
        }
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
    case LEX_STATE_READING_STRING_LITERAL_WITH_ESCAPE:
        m_context.current_token.end++;
        m_context.state = LEX_STATE_READING_STRING_LITERAL;
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
    //FIXME: Currently when there is unidentified token the lexer just ignores it.
}

void ShaderPreprocLexer::feed(const char *str) {
    size_t i = 0;
    while (str[i] != '\0') {
        feed(str[i]);
        i++;
    }
}

void ShaderPreprocLexer::finalize() {
    if (m_context.state != LEX_STATE_READING_SPACE) {
        m_tokens.push_back(m_context.current_token);
        m_context.current_token = ShaderPreprocLexerToken();
    }
}

int main(int argc, char **argv) {

    if (argc < 3) {
        std::cerr << "Specify input and output files" << std::endl;
        return 1;
    }

    std::cout << "Hello, world!\n";

    std::cout << "Output file: " << argv[2] << std::endl;
    std::cout << "Input file: " << argv[1] << std::endl;
    std::ifstream in_file(argv[1]);
    std::ofstream out_file;

    out_file.open(argv[2], std::ofstream::out);

    if (!in_file.is_open()) {
        std::cout << "Could not open input in_file\n";
        return 1;
    }

    if (!out_file.is_open()) {
        std::cout << "Could not open output file\n";
        return 1;
    }

    std::array<char, 513> rbuf = {0};

    ShaderPreprocLexer lexer;

    int iter_count = 0;

    while (!in_file.eof()) {
        std::fill(rbuf.begin(), rbuf.end(), 0);
        in_file.read(&rbuf[0], 512);

        lexer.feed(&rbuf[0]);

        iter_count++;
        if (iter_count > 10) {
            break;
        }
    }
    lexer.finalize();

    out_file << lexer << std::endl;
    out_file << "//Testing testing!\n";
    out_file << "int main() { return 0; }\n";
    out_file.close();
}
