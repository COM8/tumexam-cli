#pragma once

#include "backend/tumexam/SubmissionStudent.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::tumexam {
class Submissions {
 public:
    int announced;
    int uploaded;
    int downloaded;
    int registrations;
    std::vector<SubmissionStudent> students;

    static Submissions from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam