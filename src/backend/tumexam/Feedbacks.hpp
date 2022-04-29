#pragma once

#include "FeedbackStudent.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

namespace backend::tumexam {
class Feedbacks {
 public:
    size_t count_total{0};
    size_t count_students_downloaded{0};
    size_t count_students_with_feedback{0};
    std::vector<std::shared_ptr<FeedbackStudent>> students;

    static Feedbacks from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam