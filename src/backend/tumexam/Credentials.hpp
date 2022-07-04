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

    Credentials(std::string base_url, std::string exam, std::string session, std::string token);
    Credentials(Credentials&& old) noexcept = default;
    Credentials(const Credentials& other) noexcept = default;

    ~Credentials() = default;

    Credentials& operator=(Credentials&& old) noexcept = default;
    Credentials& operator=(const Credentials& other) noexcept = default;
};

std::shared_ptr<Credentials>* get_credentials_instance();
}  // namespace backend::tumexam