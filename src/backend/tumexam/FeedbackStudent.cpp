#include "FeedbackStudent.hpp"

namespace backend::tumexam {
FeedbackStudent FeedbackStudent::from_json(const nlohmann::json& j) {
    if (!j.contains("bgcolor")) {
        throw std::runtime_error("Failed to parse FeedbackStudent. 'bgcolor' field missing.");
    }
    std::string color;
    j.at("bgcolor").get_to(color);

    if (!j.contains("erid")) {
        throw std::runtime_error("Failed to parse FeedbackStudent. 'erid' field missing.");
    }
    std::string erid;
    j.at("erid").get_to(erid);

    if (!j.contains("registration_number")) {
        throw std::runtime_error("Failed to parse FeedbackStudent. 'registration_number' field missing.");
    }
    std::string matrikel;
    j.at("registration_number").get_to(matrikel);

    if (!j.contains("student_link")) {
        throw std::runtime_error("Failed to parse FeedbackStudent. 'student_link' field missing.");
    }
    std::string tum_exam_link;
    j.at("student_link").get_to(tum_exam_link);

    if (!j.contains("exam_id")) {
        throw std::runtime_error("Failed to parse FeedbackStudent. 'exam_id' field missing.");
    }
    int examId = 0;
    j.at("exam_id").get_to(examId);

    if (!j.contains("individual_feedback")) {
        throw std::runtime_error("Failed to parse FeedbackStudent. 'individual_feedback' field missing.");
    }
    bool has_individual_feedback_end = false;
    j.at("individual_feedback").get_to(has_individual_feedback_end);

    std::vector<Feedback> feedbacks;
    if (j.contains("feedback") && !j.at("feedback").is_null()) {
        nlohmann::json::array_t feedback_array;
        j.at("feedback").get_to(feedback_array);
        for (const nlohmann::json& jFeedback : feedback_array) {
            feedbacks.push_back(Feedback::from_json(jFeedback));
        }
    }

    bool has_downloaded = (!feedbacks.empty() || color != "#CCCCCC");

    return FeedbackStudent{
        std::move(color),
        std::move(erid),
        std::move(matrikel),
        std::move(tum_exam_link),
        examId,
        has_individual_feedback_end,
        has_downloaded,
        std::move(feedbacks)};
}
}  // namespace backend::tumexam