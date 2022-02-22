#include "Submission.hpp"
#include "ParseUtils.hpp"
#include "backend/tumexam/SubmissionMetadata.hpp"
#include <string>

namespace backend::tumexam {
Submission Submission::from_json(const nlohmann::json& j) {
    if (!j.contains("announced")) {
        throw std::runtime_error("Failed to parse Submission. 'announced' filed missing.");
    }
    bool announced = false;
    j.at("announced").get_to(announced);

    if (!j.contains("uploaded")) {
        throw std::runtime_error("Failed to parse Submission. 'uploaded' filed missing.");
    }
    bool uploaded = false;
    j.at("uploaded").get_to(uploaded);

    if (!j.contains("path")) {
        throw std::runtime_error("Failed to parse Submission. 'path' filed missing.");
    }
    std::string path;
    if (!j.at("path").is_null()) {
        j.at("path").get_to(path);
    }

    if (!j.contains("metadata")) {
        throw std::runtime_error("Failed to parse Submission. 'metadata' filed missing.");
    }
    if (!j.at("metadata").is_array()) {
        throw std::runtime_error("Failed to parse Submission. 'metadata' is no array.");
    }
    nlohmann::json::array_t metadata_array;
    j.at("metadata").get_to(metadata_array);
    std::vector<SubmissionMetadata> metadata;
    for (const nlohmann::json& jMetadata : metadata_array) {
        metadata.push_back(SubmissionMetadata::from_json(jMetadata));
    }

    return Submission{
        parse_time(j, "upload_date", "Submission"),
        parse_time(j, "announce_date", "Submission"),
        announced,
        uploaded,
        std::move(path),
        std::move(metadata)};
}
}  // namespace backend::tumexam