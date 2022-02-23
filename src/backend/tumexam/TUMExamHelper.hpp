#pragma once

#include "Credentials.hpp"
#include "Submissions.hpp"
#include <optional>
#include <string>

namespace backend::tumexam {
std::optional<Submissions> get_submission_status(const Credentials& credentials);
}  // namespace backend::tumexam