#include "Credentials.hpp"
#include <memory>

namespace backend::tumexam {
std::shared_ptr<Credentials>* get_credentials_instance() {
    static std::shared_ptr<Credentials> instance = nullptr;
    return &instance;
}
}  // namespace backend::tumexam