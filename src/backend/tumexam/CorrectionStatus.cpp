#include "CorrectionStatus.hpp"
#include <optional>

namespace backend::tumexam {
CorrectionStatus CorrectionStatus::from_json(const nlohmann::json& j) {
    if (!j.contains("available")) {
        throw std::runtime_error("Failed to parse CorrectionStatus. 'available' filed missing.");
    }
    int available = 0;
    j.at("available").get_to(available);

    if (!j.contains("corrected")) {
        throw std::runtime_error("Failed to parse CorrectionStatus. 'corrected' filed missing.");
    }
    int corrected = 0;
    j.at("corrected").get_to(corrected);

    if (!j.contains("problem")) {
        throw std::runtime_error("Failed to parse CorrectionStatus. 'problem' filed missing.");
    }
    int problem = 0;
    j.at("problem").get_to(problem);

    if (!j.contains("correction_pass")) {
        throw std::runtime_error("Failed to parse CorrectionStatus. 'correction_pass' filed missing.");
    }
    int pass = 0;
    j.at("correction_pass").get_to(pass);

    if (!j.contains("total")) {
        throw std::runtime_error("Failed to parse CorrectionStatus. 'total' filed missing.");
    }
    int total = 0;
    j.at("total").get_to(total);

    // Only available for subproblems:
    std::optional<std::string> subproblem = std::nullopt;
    if (j.contains("subproblem") && !j.at("subproblem").is_null()) {
        std::string s;
        j.at("subproblem").get_to(s);
        subproblem = std::make_optional(std::move(s));
    }

    return CorrectionStatus{
        total,
        available,
        corrected,
        pass,
        problem,
        std::move(subproblem)};
}
}  // namespace backend::tumexam