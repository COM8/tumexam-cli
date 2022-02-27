#include "StringUtils.hpp"

namespace utils {
void ltrim(std::string_view& s) {
    s.remove_prefix(std::distance(s.cbegin(), std::find_if(s.cbegin(), s.cend(), std::not1(std::ptr_fun<int, int>(std::isspace)))));
}

void rtrim(std::string_view& s) {
    s.remove_suffix(std::distance(s.crbegin(), std::find_if(s.crbegin(), s.crend(), std::not1(std::ptr_fun<int, int>(std::isspace)))));
}

std::string to_lower_clean(const std::string& s) {
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) {
        if (std::isspace(c)) {
            return ' ';
        }
        return static_cast<char>(std::tolower(c));
    });
    return tmp;
}

void trim(std::string_view& s) {
    ltrim(s);
    rtrim(s);
}
}  // namespace utils