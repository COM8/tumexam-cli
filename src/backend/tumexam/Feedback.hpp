#pragma once

#include <chrono>
#include <nlohmann/json.hpp>

namespace backend::tumexam {
class Feedback {
 public:
    int problem{0};
    std::string sub_problem;
    std::string text;
    std::chrono::system_clock::time_point time_stamp;

    static Feedback from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam