#include "SubmissionStudent.hpp"
#include "backend/tumexam/ExamSubmissionDetails.hpp"
#include "backend/tumexam/Submission.hpp"
#include "logger/Logger.hpp"
#include <exception>
#include <optional>
#include <stdexcept>
#include <vector>

namespace backend::tumexam {
SubmissionStudent SubmissionStudent::from_json(const nlohmann::json& j) {
    if (!j.contains("announced")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'announced' field missing.");
    }
    bool announced = false;
    j.at("announced").get_to(announced);

    if (!j.contains("downloaded")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'downloaded' field missing.");
    }
    bool downloaded = false;
    j.at("downloaded").get_to(downloaded);

    if (!j.contains("uploaded")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'uploaded' field missing.");
    }
    bool uploaded = false;
    j.at("uploaded").get_to(uploaded);

    if (!j.contains("additionalTimeMinutes")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'additionalTimeMinutes' field missing.");
    }
    std::optional<int> additional_time_minutes = std::nullopt;
    if (!j.at("additionalTimeMinutes").is_null()) {
        int i = 0;
        j.at("additionalTimeMinutes").get_to(i);
        additional_time_minutes = std::make_optional<int>(i);
    }

    if (!j.contains("exam")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'exam' field missing.");
    }
    std::optional<ExamSubmissionDetails> exam_submission_details = std::nullopt;
    if (!j.at("exam").is_null()) {
        ExamSubmissionDetails exDetails = ExamSubmissionDetails::from_json(j.at("exam"));
        exam_submission_details = std::make_optional<ExamSubmissionDetails>(exDetails);
    }

    if (!j.contains("matrikel")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'matrikel' field missing.");
    }
    std::string matrikel;
    j.at("matrikel").get_to(matrikel);

    if (!j.contains("submissionNote")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'submissionNote' field missing.");
    }
    std::string submissionNote;
    if (!j.at("submissionNote").is_null()) {
        j.at("submissionNote").get_to(submissionNote);
    }

    if (!j.contains("submissions")) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'submissions' field missing.");
    }
    if (!j.at("submissions").is_array()) {
        throw std::runtime_error("Failed to parse SubmissionStudent. 'submissions' is no array.");
    }
    nlohmann::json::array_t submissions_array;
    j.at("submissions").get_to(submissions_array);
    std::vector<Submission> submissions;
    for (const nlohmann::json& jSubmission : submissions_array) {
        submissions.push_back(Submission::from_json(jSubmission));
    }

    return SubmissionStudent{
        announced,
        downloaded,
        uploaded,
        std::move(matrikel),
        std::move(submissionNote),
        std::move(submissions),
        additional_time_minutes,
        exam_submission_details};
}

int SubmissionStudent::get_state() const {
    if (downloaded) {
        if (uploaded) {
            if (announced) {
                return 3;
            }
            return 2;
        }
        if (announced) {
            return 4;
        }
        return 1;
    }
    return 0;
}
}  // namespace backend::tumexam