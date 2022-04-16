#pragma once

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

namespace backend::tumexam {
std::chrono::system_clock::time_point parse_time(const nlohmann::json& j, const std::string& name, const std::string& class_name);
std::chrono::system_clock::time_point parse_correction_time(const nlohmann::json& j, const std::string& name, const std::string& class_name);
}  // namespace backend::tumexam