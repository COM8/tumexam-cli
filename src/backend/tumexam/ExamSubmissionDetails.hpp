#pragma once

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

namespace backend::tumexam {
class ExamSubmissionDetails {
 public:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    std::chrono::system_clock::time_point submission_end;
    std::chrono::system_clock::time_point upload_end;

    static ExamSubmissionDetails from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam