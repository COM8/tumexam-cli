#pragma once

#include <cstddef>
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
    /**
     * Only used in problem view to show how many exams already have been corrected completely.
     * Else it's the same like corrected.
     **/
    int corrected_min{0};
    int subproblem_count{0};

    static CorrectionPass from_json(const nlohmann::json& j);

    CorrectionPass& operator+=(const CorrectionPass& other);
};

}  // namespace backend::tumexam