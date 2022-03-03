#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace backend::tumexam {
class CorrectionPass {
 public:
    int pass{0};
    int total{0};
    int available{0};
    int corrected{0};

    static CorrectionPass from_json(const nlohmann::json& j);
};

}  // namespace backend::tumexam