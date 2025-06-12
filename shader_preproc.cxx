#include "shader_preproc.hxx"
#include <algorithm>
#include <array>
#include <fstream>
#include <hash>
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
    file.close();
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
}

ShaderPreprocLexer::ShaderPreprocLexer() : m_tokens(), m_context() {}

bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n'; }

bool is_digit(char c) { return '0' >= c && c <= '9'; }

bool is_alphabetic(char c) {
    return ('a' >= c && c <= 'z') || ('A' >= c && c <= 'Z');
}

// TODO: Create a table of hashes for all the keywords
// match identifiers against the hashes to change them to keywords.

bool is_keyword(const std::string &str) {

    static bool first_time = true;
    std::vector<decltype(std::hash<std::string>{}(std::string()))>
        keyword_hashes;

    if (first_time) {
        keyword_hashes.push_back(std::hash<std::string>{}(std::string("if")));
        keyword_hashes.push_back(std::hash<std::string>{}(std::string("else")));
        keyword_hashes.push_back(
            std::hash<std::string>{}(std::string("switch")));
        keyword_hashes.push_back(std::hash<std::string>{}(std::string("for")));
        keyword_hashes.push_back(
            std::hash<std::string>{}(std::string("struct")));
        keyword_hashes.push_back(
            std::hash<std::string>{}(std::string("layout")));
        keyword_hashes.push_back(
            std::hash<std::string>{}(std::string("attribute")));
        std::sort(keyword_hashes.begin(), keyword_hashes.end());

        first_time = false;
    }

    auto h = std::hash<std::string>{}(str);

    if (std::binary_search(keyword_hashes.begin(), keyword_hashes.end(), h)) {
        return true;
    }

    return false;
}

bool is_punctuation(char c) {
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

    if (is_space(c) && m_context.state != LEX_STATE_READING_SPACE) {
        m_tokens.push_back(m_context.current_token);
        m_context.state = LEX_STATE_READING_IDENTIFIER;
    }

    if (is_space(c)) {
        m_context.position++;
        return;
    }

    auto start_new_tok = [&](ShaderTokenType type) {
        m_context.current_token = {.begin = m_context.position,
                                   .end = m_context.position + 1,
                                   .type = type};
    };

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
            m_context.keyword_check_str.push_back(c);
        } else if (is_punctuation(c)) {
            start_new_tok(LEX_TOK_IDENTIFIER);
            m_context.current_token.end++;
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
        m_context.current_token.end++;
        if (!(is_alphabetic(c) || is_digit(c) || c == '_')) {
            if (is_keyword(m_context.keyword_check_str)) {
                m_context.current_token.type = LEX_TOK_KEYWORD;
            };
            m_tokens.push_back(m_context.current_token);
            m_context.current_token = ShaderPreprocLexerToken();
            m_context.keyword_check_str.clear();
        } else {
            m_context.keyword_check_str.push_back(c);
        }
        break;
    }

    m_context.position++;
}

void ShaderPreprocLexer::feed(const char *str) {
    size_t i = 0;
    while (str[i] != '\0') {
        feed(str[i]);
    }
}

void ShaderPreprocLexer::finalize() {
    m_tokens.push_back(m_context.current_token);
    m_context.current_token = ShaderPreprocLexerToken();
}

int main(int argc, char **argv) {

    if (argc < 3) {
        std::cerr << "Specify input and output files" << std::endl;
        return 1;
    }

    std::ifstream in_file(argv[1]);
    std::ofstream out_file(argv[2]);

    if (!in_file.is_open()) {
        std::cerr << "Could not open input in_file\n";
        return 1;
    }

    std::array<char, 513> rbuf = {0};

    ShaderPreprocLexer lexer;

    while (!in_file.eof()) {
        std::fill(rbuf.begin(), rbuf.end(), 0);
        in_file.read(&rbuf[0], 512);

        lexer.feed(&rbuf[0]);
    }
    lexer.finalize();
    out_file.close();

    out_file << lexer << std::endl;
    out_file.close();
}
