#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>

const size_t READ_BUFFER_SIZE = 512;
const size_t BYTES_PER_LINE = 16;

void print_help(const std::string& executable_name);

int main(int argc, char **argv) {

    if (argc < 3) {
        print_help(argv[0]);
    }

    std::array<char, READ_BUFFER_SIZE> read_buffer;
    size_t read_count = 0;

    std::string executable_name(argv[0]);
    int arg_count = (argc - 1) / 2;
    char** args = &argv[1];

    std::cout << "arg_count = " << arg_count << std::endl;

    for (int i = 0; i < arg_count; i += 1) {
        std::string filename(args[i * 2]);
        std::string buffer_name(args[(i * 2 + 1)]);
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        size_t total_bytes = 0;

        if (!file.is_open() || file.bad()) {
            std::cout << "Invalid file: " << filename << std::endl;
            exit(1);
        }

        std::cout << "/* FILE: " << filename << " */\n";
        std::cout << "const char " << buffer_name << "[] = {\n";

        while (!file.eof()) {
            uint8_t byte = 0;
            read_count = 0;
            std::fill(read_buffer.begin(), read_buffer.end(), 0);
            file.read(&read_buffer[0], read_buffer.size());
            read_count = file.gcount();

            size_t remaning_bytes_on_first_line = total_bytes % BYTES_PER_LINE;

            auto print_byte = [](char a) {
                std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<uint32_t>(a) << ", ";
            };

            // Complete unfinished line if any
            for (int x = 0;
                 x < std::min(remaning_bytes_on_first_line, read_count); x++) {
                print_byte(read_buffer[x]);
                total_bytes++;
            }

            std::cout << "\n";

            // Write Complete lines
            for (int y = 0; y < ((read_count - remaning_bytes_on_first_line) %
                                 BYTES_PER_LINE);
                 y++) {
                for (int x = 0; x < BYTES_PER_LINE; x++) {
                    print_byte(read_buffer[remaning_bytes_on_first_line +
                                           (y * BYTES_PER_LINE) + x]);
                    total_bytes++;
                }
                std::cout << "\n";
            }

            int bytes_writen_this_iteration = remaning_bytes_on_first_line +
                ((read_count - remaning_bytes_on_first_line) % BYTES_PER_LINE);
            int remaning_byte_on_last_line = read_count - bytes_writen_this_iteration;

            for (int x = 0; x < remaning_bytes_on_first_line; x++) {
                print_byte(read_buffer[x+bytes_writen_this_iteration]);
                total_bytes++;
            }
        }

        std::cout << "\n};\n\n";
        file.close();
    }
}



void print_help(const std::string& executable_name) {
    std::cout << executable_name << " <file_name> <buffer_name> ..." << std::endl;
}
