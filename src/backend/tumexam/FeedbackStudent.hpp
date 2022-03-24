#pragma once

#include "Feedback.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::tumexam {
class FeedbackStudent {
 public:
    std::string color;
    std::string erid;
    std::string matrikel;
    std::string tum_exam_link;
    int examId{0};
    bool has_individual_feedback_end{false};
    bool has_downloaded{false};
    std::vector<Feedback> feedbacks;

    static FeedbackStudent from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam