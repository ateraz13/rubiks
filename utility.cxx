#include "utility.hxx"
#include <fstream>

std::optional<std::string> read_text_file(const std::string& path) {

  std::string text, line;

  std::ifstream file(path, std::ifstream::binary);

  if(!file.is_open()) {
    return {};
  }

  file.seekg(0, file.end);
  int file_len = file.tellg();
  file.seekg(0, file.beg);

  text.reserve(file_len);

  using siter = std::istreambuf_iterator<char>;
  text.assign(siter(file), siter());

  file.close();
  return text;
}
