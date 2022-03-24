#pragma once

#include "CorrectionStatus.hpp"
#include "Credentials.hpp"
#include "Feedbacks.hpp"
#include "Submissions.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace backend::tumexam {
std::optional<Submissions> get_submission_status(const Credentials& credentials);
std::vector<std::shared_ptr<CorrectionStatus>> get_correction_status(const Credentials& credentials, bool subproblems);
std::optional<Feedbacks> get_feedbacks(const Credentials& credentials);
}  // namespace backend::tumexam