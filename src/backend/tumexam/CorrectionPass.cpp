#include "CorrectionPass.hpp"
#include <optional>

namespace backend::tumexam {
CorrectionPass CorrectionPass::from_json(const nlohmann::json& j) {
    if (!j.contains("correction_pass")) {
        throw std::runtime_error("Failed to parse CorrectionPass. 'correction_pass' field missing.");
    }
    int pass = 0;
    j.at("correction_pass").get_to(pass);

    if (!j.contains("total")) {
        throw std::runtime_error("Failed to parse CorrectionPass. 'total' field missing.");
    }
    int total = 0;
    j.at("total").get_to(total);

    if (!j.contains("available")) {
        throw std::runtime_error("Failed to parse CorrectionPass. 'available' field missing.");
    }
    int available = 0;
    j.at("available").get_to(available);

    if (!j.contains("corrected")) {
        throw std::runtime_error("Failed to parse CorrectionPass. 'corrected' field missing.");
    }
    int corrected = 0;
    j.at("corrected").get_to(corrected);

    return CorrectionPass{
        pass,
        total,
        available,
        corrected};
}
}  // namespace backend::tumexam