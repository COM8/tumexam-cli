#pragma once

#include <string>

namespace backend::tumexam {
class Credentials {
 public:
    std::string base_url;
    std::string exam;
    std::string session;
    std::string token;
};
}  // namespace backend::tumexam