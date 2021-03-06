#pragma once

#include "Correction.hpp"
#include "CorrectionStatus.hpp"
#include "Credentials.hpp"
#include "Feedbacks.hpp"
#include "Student.hpp"
#include "Submissions.hpp"
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace backend::tumexam {
std::optional<Submissions> get_submission_status(const Credentials& credentials);
std::vector<std::shared_ptr<CorrectionStatus>> get_correction_status(const Credentials& credentials, bool subproblems);
std::optional<Feedbacks> get_feedbacks(const Credentials& credentials);
std::shared_ptr<Credentials> login(const std::string& instance, const std::string& username, const std::string& password);
std::vector<Student> get_students(const Credentials& credentials);
std::vector<std::shared_ptr<backend::tumexam::Correction>> get_corrections_since(const Credentials& credentials, std::chrono::system_clock::time_point since, size_t max);
}  // namespace backend::tumexam