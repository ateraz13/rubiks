#ifndef EXCEPT_HXX
#define EXCEPT_HXX
#include <stdexcept>

class NotImplemented : std::runtime_error {
    using std::runtime_error::runtime_error;
};

#endif // EXCEPT_HXX
