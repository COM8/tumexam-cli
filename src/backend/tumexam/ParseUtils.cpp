#include "ParseUtils.hpp"
#include "utils/Date.hpp"
#include <exception>

namespace backend::tumexam {
std::chrono::system_clock::time_point parse_time(const nlohmann::json& j, const std::string& name, const std::string& class_name, const std::string& format) {
    if (!j.contains(name)) {
        throw std::runtime_error("Failed to parse " + class_name + ". '" + name + "' field missing.");
    }
    if (j.at(name).is_null()) {
        return std::chrono::system_clock::time_point::min();
    }
    std::string str;
    j.at(name).get_to(str);

    std::string::size_type pos = str.find('.');
    if (pos != std::string::npos) {
        str = str.substr(0, pos);
    }

    std::chrono::system_clock::time_point tp;
    std::istringstream ss{str};
    ss >> date::parse(format, tp);
    return tp;
}

std::chrono::system_clock::time_point parse_time(const nlohmann::json& j, const std::string& name, const std::string& class_name) {
    return parse_time(j, name, class_name, "%a, %d %b %Y %H:%M:%S %Z");
}

std::chrono::system_clock::time_point parse_correction_time(const nlohmann::json& j, const std::string& name, const std::string& class_name) {
    return parse_time(j, name, class_name, "%Y-%m-%dT%H:%M:%S");
}

// 2022-02-16T20:50:45.876893
}  // namespace backend::tumexam