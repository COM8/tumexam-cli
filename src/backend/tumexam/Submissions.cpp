#include "Submissions.hpp"
#include "backend/tumexam/SubmissionStudent.hpp"
#include "nlohmann/json.hpp"
#include <memory>

namespace backend::tumexam {
Submissions Submissions::from_json(const nlohmann::json& j) {
    if (!j.contains("statistics")) {
        throw std::runtime_error("Failed to parse Submissions. 'statistics' field missing.");
    }
    const nlohmann::json statistics = j.at("statistics");

    if (!statistics.contains("announced")) {
        throw std::runtime_error("Failed to parse Submissions. 'announced' field missing.");
    }
    int announced = 0;
    statistics.at("announced").get_to(announced);

    if (!statistics.contains("uploaded")) {
        throw std::runtime_error("Failed to parse Submissions. 'uploaded' field missing.");
    }
    int uploaded = false;
    statistics.at("uploaded").get_to(uploaded);

    if (!statistics.contains("downloaded")) {
        throw std::runtime_error("Failed to parse Submissions. 'downloaded' field missing.");
    }
    int downloaded = false;
    statistics.at("downloaded").get_to(downloaded);

    if (!statistics.contains("registrations")) {
        throw std::runtime_error("Failed to parse Submissions. 'registrations' field missing.");
    }
    int registrations = false;
    statistics.at("registrations").get_to(registrations);

    if (!j.contains("students")) {
        throw std::runtime_error("Failed to parse Submissions. 'students' field missing.");
    }
    if (!j.at("students").is_array()) {
        throw std::runtime_error("Failed to parse Submissions. 'students' is no array.");
    }
    nlohmann::json::array_t students_array;
    j.at("students").get_to(students_array);
    std::vector<std::shared_ptr<SubmissionStudent>> students;
    for (const nlohmann::json& jStudent : students_array) {
        students.emplace_back(std::make_unique<SubmissionStudent>(SubmissionStudent::from_json(jStudent)));
    }

    return Submissions{
        announced,
        uploaded,
        downloaded,
        registrations,
        std::move(students)};
}
}  // namespace backend::tumexam