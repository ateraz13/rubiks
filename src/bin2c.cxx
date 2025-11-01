#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

const size_t READ_BUFFER_SIZE = 512;
const size_t BYTES_PER_LINE = 16;

void print_help(const std::string &executable_name);

struct CmdArg {
    std::string filename;
    std::string buffer_name;

    CmdArg(const char *fn, const char *bn) : filename(fn), buffer_name(bn) {}

    CmdArg() = default;
    CmdArg(const CmdArg &) = default;
    CmdArg(CmdArg &&) = default;
    CmdArg &operator=(const CmdArg &) = default;
    CmdArg &operator=(CmdArg &&) = default;
};

struct CmdLine {
    std::string executable_name;
    std::optional<std::string> output_filename;
    std::vector<CmdArg> cmd_args;

    CmdLine() = default;
    CmdLine(const CmdLine &) = default;
    CmdLine(CmdLine &&) = default;
    CmdLine &operator=(const CmdLine &) = default;
    CmdLine &operator=(CmdLine &&) = default;

    void parse(int argc, char **argv);
};

struct MalformedCmd : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

enum CmdLineState {
    CMDLN_EXPECT_INPUT_FILENAME = 0,
    CMDLN_EXPECT_BUFFER_NAME,
    CMDLN_EXPECT_OUTPUT_FILENAME,
};

void CmdLine::parse(int argc, char **argv) {
    if (argc < 4) {
        throw MalformedCmd("Wrong number of arguments to command!");
    }

    executable_name = std::string(argv[0]);

    CmdLineState state = CMDLN_EXPECT_INPUT_FILENAME;

    CmdArg current_arg;

    for (int i = 1; i < argc; i++) {

        if (state == CMDLN_EXPECT_OUTPUT_FILENAME) {
            if (argv[i][0] != 0) {
                output_filename = std::string(argv[i]);
            }
            state = CMDLN_EXPECT_INPUT_FILENAME;
            continue;
        }

        if (argv[i] && argv[i][0] == '-') {
            if (state == CMDLN_EXPECT_BUFFER_NAME) {
                throw MalformedCmd(
                    "Expected buffer name but found option directive instead!");
            }

            if (argv[i][1] != 'o' && argv[i][1] != 0) {
                std::stringstream ss;
                ss << "Invalid command line optio \"-" << argv[i][1];
                throw MalformedCmd(ss.str());
            }

            state = CMDLN_EXPECT_OUTPUT_FILENAME;
            continue;
        }

        if (state == CMDLN_EXPECT_BUFFER_NAME) {
            current_arg.buffer_name = std::string(argv[i]);
            cmd_args.push_back(current_arg);
            current_arg = CmdArg();
            state = CMDLN_EXPECT_INPUT_FILENAME;
            continue;
        }

        if (state == CMDLN_EXPECT_INPUT_FILENAME) {
            current_arg.filename = std::string(argv[i]);
            state = CMDLN_EXPECT_BUFFER_NAME;
            continue;
        }
    }
}

int main(int argc, char **argv) {

    CmdLine cmd_line;

    try {
        cmd_line.parse(argc, argv);
    } catch (MalformedCmd err) {
        std::cerr << err.what() << "\n";
        print_help(argv[0]);
        exit(1);
    }

    for (auto arg : cmd_line.cmd_args) {
        std::cout << "filename = " << arg.filename
                  << ", buffer_name = " << arg.buffer_name << std::endl;
    }

    if (cmd_line.output_filename) {
        std::cout << "Output filename = " << *cmd_line.output_filename
                  << std::endl;
    }

    std::array<char, READ_BUFFER_SIZE> read_buffer;
    size_t read_count = 0;

    std::ostream *ostrm = &std::cout;
    std::fstream output_file;

    if (cmd_line.output_filename) {
        output_file.open(*cmd_line.output_filename, std::fstream::out);
        ostrm = &output_file;
    }

    for (auto arg : cmd_line.cmd_args) {
        std::ifstream file(arg.filename, std::ios::in | std::ios::binary);
        size_t total_bytes = 0;

        if (!file.is_open() || file.bad()) {
            std::cout << "Invalid file: " << arg.filename << std::endl;
            exit(1);
        }

        std::cout << "/* FILE: " << arg.filename << " */\n";
        *ostrm << "const char " << arg.buffer_name << "[] = {\n";

        while (!file.eof()) {
            uint8_t byte = 0;
            read_count = 0;
            std::fill(read_buffer.begin(), read_buffer.end(), 0);
            file.read(&read_buffer[0], read_buffer.size());
            read_count = file.gcount();

            size_t remaning_bytes_on_first_line = total_bytes % BYTES_PER_LINE;

            size_t local_total_bytes = 0;

            auto print_byte = [&](char a) {
                std::string rep;
                switch (a) {
                case ' ':
                    rep = "\\s ";
                    break;
                case '\t':
                    rep = "\\ţ ";
                    break;
                case '\n':
                    rep = "\\n ";
                    break;
                default:
                    const char s[] = {' ', a, ' ', 0x00};
                    rep = s;
                    break;
                }
                *ostrm << "0x" << std::setw(2) << std::setfill('0') << std::hex
                       << static_cast<uint32_t>(a) << "/* " << rep << " */, ";
                total_bytes++;
                local_total_bytes++;
            };

            for(int x = 0; x != read_count; x++) {
                print_byte(read_buffer[x]);
                if(total_bytes % BYTES_PER_LINE == 0) {
                    *ostrm << "\n";
                }
            }
        }

        *ostrm << "\n};\n\n";
        file.close();
    }
    output_file.close();
}

void print_help(const std::string &executable_name) {
    std::cout << executable_name
              << " <file_name> <buffer_name> ...  [-o <output_file>]"
              << std::endl;
}
