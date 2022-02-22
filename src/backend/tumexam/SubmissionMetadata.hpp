#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace backend::tumexam {
class SubmissionMetadata {
 public:
    std::string mime_type;
    std::string name;
    std::string sha256;
    int size;

    static SubmissionMetadata from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam