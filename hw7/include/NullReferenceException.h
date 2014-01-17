#include <stdexcept>
#include <string>

class NullReferenceException : public std::runtime_error { 
public:
    NullReferenceException() : std::runtime_error("deferencing a null pointer") {}
    NullReferenceException(const std::string& s) : std::runtime_error(s) {}
    ~NullReferenceException() throw () {}
};