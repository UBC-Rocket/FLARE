// Purpose of this file is to help separate implementation from interface
#include <memory> // for std::unique_ptr
#include <vector> // for std::vector
typedef std::unique_ptr<std::vector<uint8_t>> SubPktPtr;
