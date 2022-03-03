#include "SubmissionMetadata.hpp"
#include <string>

namespace backend::tumexam {
SubmissionMetadata SubmissionMetadata::from_json(const nlohmann::json& j) {
    if (!j.contains("mimetype")) {
        throw std::runtime_error("Failed to parse SubmissionMetadata. 'mimetype' field missing.");
    }
    std::string mime_type;
    j.at("mimetype").get_to(mime_type);

    if (!j.contains("name")) {
        throw std::runtime_error("Failed to parse SubmissionMetadata. 'name' field missing.");
    }
    std::string name;
    j.at("name").get_to(name);

    if (!j.contains("sha256")) {
        throw std::runtime_error("Failed to parse SubmissionMetadata. 'sha256' field missing.");
    }
    std::string sha256;
    j.at("sha256").get_to(sha256);

    if (!j.contains("size")) {
        throw std::runtime_error("Failed to parse SubmissionMetadata. 'size' field missing.");
    }
    int size = 0;
    j.at("size").get_to(size);

    return SubmissionMetadata{
        std::move(mime_type),
        std::move(name),
        std::move(sha256),
        size};
}
}  // namespace backend::tumexam