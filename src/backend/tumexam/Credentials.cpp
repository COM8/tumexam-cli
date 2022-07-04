#include "Credentials.hpp"
#include <memory>

namespace backend::tumexam {
Credentials::Credentials(std::string base_url, std::string exam, std::string session, std::string token) : base_url(std::move(base_url)),
                                                                                                           exam(std::move(exam)),
                                                                                                           session(std::move(session)),
                                                                                                           token(std::move(token)) {}

std::shared_ptr<Credentials>* get_credentials_instance() {
    static std::shared_ptr<Credentials> instance = nullptr;
    return &instance;
}
}  // namespace backend::tumexam