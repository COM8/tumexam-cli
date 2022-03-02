#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace backend::tumexam {
class CorrectionStatus {
 public:
    int total{0};
    int available{0};
    int corrected{0};
    int pass{0};
    int problem{0};
    std::optional<std::string> subproblem{std::nullopt};

    static CorrectionStatus from_json(const nlohmann::json& j);
};

}  // namespace backend::tumexam