#include "Feedbacks.hpp"
#include "FeedbackStudent.hpp"
#include <cstddef>
#include <memory>

namespace backend::tumexam {
Feedbacks Feedbacks::from_json(const nlohmann::json& j) {
    if (!j.contains("erid")) {
        throw std::runtime_error("Failed to parse Feedbacks. 'erid' field missing.");
    }

    nlohmann::json::array_t erid_array;
    j.at("erid").get_to(erid_array);
    std::vector<std::shared_ptr<FeedbackStudent>> students;
    size_t count_total = 0;
    size_t count_students_with_feedback = 0;
    size_t count_students_downloaded = 0;
    for (const nlohmann::json& jStudent : erid_array) {
        students.push_back(std::make_shared<FeedbackStudent>(FeedbackStudent::from_json(jStudent)));
        if (!students.back()->feedbacks.empty()) {
            count_students_with_feedback++;
            count_total += students.back()->feedbacks.size();
        }
        if (students.back()->has_downloaded) {
            count_students_downloaded++;
        }
    }

    return Feedbacks{
        count_total,
        count_students_downloaded,
        count_students_with_feedback,
        std::move(students)};
}
}  // namespace backend::tumexam