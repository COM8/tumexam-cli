#pragma once

#include <chrono>
#include <nlohmann/json.hpp>

namespace backend::tumexam {
class Correction {
 public:
    int correction_pass{0};
    std::string problem;
    std::string sub_problem;
    std::string erid;
    std::string corrector;
    std::chrono::system_clock::time_point date;

    static Correction from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam