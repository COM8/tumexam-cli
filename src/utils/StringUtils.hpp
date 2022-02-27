#pragma once
#include <string>

namespace utils {
void ltrim(std::string_view& s);
void rtrim(std::string_view& s);
std::string to_lower_clean(const std::string& s);
void trim(std::string_view& s);
}  // namespace utils