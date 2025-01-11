#ifndef EXCEPT_HXX
#define EXCEPT_HXX
#include <stdexcept>

class NotImplemented : std::runtime_error {
    using std::runtime_error::runtime_error;
};

class WindowingLibraryInitFailed: std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SystemWindowFailedToCreate: std::runtime_error {
    using std::runtime_error::runtime_error;
};

class EventOnUnregisteredWindow: std::runtime_error {
    using std::runtime_error::runtime_error;
};

#endif // EXCEPT_HXX
