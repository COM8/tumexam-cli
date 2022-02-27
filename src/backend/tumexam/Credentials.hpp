#pragma once

#include <memory>
#include <string>

namespace backend::tumexam {
class Credentials {
 public:
    std::string base_url;
    std::string exam;
    std::string session;
    std::string token;
};

std::shared_ptr<Credentials>* get_credentials_instance();
}  // namespace backend::tumexam