#pragma once

#include "backend/tumexam/SubmissionStudent.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::tumexam {
class Submissions {
 public:
    int announced{0};
    int uploaded{0};
    int downloaded{0};
    int registrations{0};
    std::vector<SubmissionStudent> students{};

    static Submissions
    from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam