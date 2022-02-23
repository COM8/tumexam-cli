#pragma once

#include "SubmissionMetadata.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::tumexam {
class Submission {
 public:
    std::chrono::system_clock::time_point upload_date;
    std::chrono::system_clock::time_point announce_date;
    bool announced{false};
    bool uploaded{false};
    std::string path;
    std::vector<SubmissionMetadata> metadata;

    static Submission from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam