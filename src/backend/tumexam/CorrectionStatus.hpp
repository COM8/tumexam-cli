#pragma once

#include "CorrectionPass.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace backend::tumexam {
class CorrectionStatus {
 public:
    int problem{0};
    std::optional<std::string> subproblem{std::nullopt};
    std::optional<CorrectionPass> pass1;
    std::optional<CorrectionPass> pass2;

    static CorrectionStatus from_json(const nlohmann::json& j);

    bool operator==(const CorrectionStatus& other) const;
    bool operator!=(const CorrectionStatus& other) const;
};

}  // namespace backend::tumexam