#include "Correction.hpp"
#include "ParseUtils.hpp"

namespace backend::tumexam {
Correction Correction::from_json(const nlohmann::json& j) {
    if (!j.contains("correction_pass")) {
        throw std::runtime_error("Failed to parse Correction. 'correction_pass' field missing.");
    }
    int correction_pass = 0;
    j.at("correction_pass").get_to(correction_pass);

    if (!j.contains("problem")) {
        throw std::runtime_error("Failed to parse Correction. 'problem' field missing.");
    }
    std::string problem;
    j.at("problem").get_to(problem);

    if (!j.contains("subproblem")) {
        throw std::runtime_error("Failed to parse Correction. 'subproblem' field missing.");
    }
    std::string subproblem;
    j.at("subproblem").get_to(subproblem);

    if (!j.contains("erid")) {
        throw std::runtime_error("Failed to parse Correction. 'erid' field missing.");
    }
    std::string erid;
    j.at("erid").get_to(erid);

    if (!j.contains("corrector")) {
        throw std::runtime_error("Failed to parse Correction. 'corrector' field missing.");
    }
    std::string corrector;
    if (!j.at("corrector").is_null()) {
        j.at("corrector").get_to(corrector);
    }

    return Correction{
        correction_pass,
        std::move(problem),
        std::move(subproblem),
        std::move(erid),
        std::move(corrector),
        parse_correction_time(j, "date", "Correction")};
}
}  // namespace backend::tumexam