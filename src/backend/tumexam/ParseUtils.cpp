#include "ParseUtils.hpp"
#include "utils/Date.hpp"
#include <exception>

namespace backend::tumexam {
std::chrono::system_clock::time_point parse_time(const nlohmann::json& j, const std::string& name, const std::string& class_name) {
    if (!j.contains(name)) {
        throw std::runtime_error("Failed to parse " + class_name + ". '" + name + "' field missing.");
    }
    if (j.at(name).is_null()) {
        return std::chrono::system_clock::time_point::min();
    }
    std::string str;
    j.at(name).get_to(str);
    std::chrono::system_clock::time_point tp;
    std::istringstream ss{str};
    ss >> date::parse("%a, %d %b %Y %H:%M:%S %Z", tp);
    return tp;
}
}  // namespace backend::tumexam