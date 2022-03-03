#include "CorrectionStatus.hpp"
#include <optional>

namespace backend::tumexam {
CorrectionStatus CorrectionStatus::from_json(const nlohmann::json& j) {
    if (!j.contains("problem")) {
        throw std::runtime_error("Failed to parse CorrectionStatus. 'problem' filed missing.");
    }
    int problem = 0;
    j.at("problem").get_to(problem);

    // Only available for subproblems:
    std::optional<std::string> subproblem = std::nullopt;
    if (j.contains("subproblem") && !j.at("subproblem").is_null()) {
        std::string s;
        j.at("subproblem").get_to(s);
        subproblem = std::make_optional(std::move(s));
    }

    return CorrectionStatus{
        problem,
        std::move(subproblem),
        std::nullopt,
        std::nullopt};
}

bool CorrectionStatus::operator==(const CorrectionStatus& other) const {
    if (problem == other.problem) {
        if (subproblem == other.subproblem) {
            return true;
        }

        return subproblem && other.subproblem && (*subproblem == *(other.subproblem));
    }
    return false;
}

bool CorrectionStatus::operator!=(const CorrectionStatus& other) const {
    return !(*this == other);
}
}  // namespace backend::tumexam