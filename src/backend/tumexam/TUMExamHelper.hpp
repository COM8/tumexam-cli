#pragma once

#include "Submissions.hpp"
#include <optional>
#include <string>

namespace backend::tumexam {
std::optional<Submissions> get_submission_status(const std::string& base_url, const std::string& exam, const std::string& session, const std::string& token);
}  // namespace backend::tumexam