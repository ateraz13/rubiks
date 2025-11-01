#ifndef UTILITY_HXX
#define UTILITY_HXX
#include <utility>
#include <optional>
#include <string>

#define EXPR_LOG(expr) std::clog << (#expr) << " = " << (expr) << std::endl;
#define ITER_LOG(container)                                                    \
  {                                                                            \
    std::clog << (#container) << " = ";                                        \
    ITER_LOG_INNER(container);                                                 \
    std::cout << std::endl;                                                    \
  }

std::optional<std::string> read_text_file(const std::string& path);

#endif // UTILITY_HXX
