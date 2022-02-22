#pragma once

#include "ExamSubmissionDetails.hpp"
#include "Submission.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace backend::tumexam {
class SubmissionStudent {
 public:
    bool announced;
    bool downloaded;
    bool uploaded;
    std::string matrikel;
    std::string submissionNote;
    std::vector<Submission> submissions;
    std::optional<int> additional_time_minutes;
    std::optional<ExamSubmissionDetails> exam_submission_details;

    static SubmissionStudent from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam