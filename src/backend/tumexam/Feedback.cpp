#include "Feedback.hpp"
#include "ParseUtils.hpp"

namespace backend::tumexam {
Feedback Feedback::from_json(const nlohmann::json& j) {
    if (!j.contains("problem")) {
        throw std::runtime_error("Failed to parse Feedback. 'problem' field missing.");
    }
    int problem = 0;
    j.at("problem").get_to(problem);

    if (!j.contains("subproblem")) {
        throw std::runtime_error("Failed to parse Feedback. 'subproblem' field missing.");
    }
    std::string subproblem;
    j.at("subproblem").get_to(subproblem);

    if (!j.contains("text")) {
        throw std::runtime_error("Failed to parse Feedback. 'text' field missing.");
    }
    std::string text;
    j.at("text").get_to(text);

    return Feedback{
        problem,
        std::move(subproblem),
        std::move(text),
        parse_time(j, "timestamp", "Feedback")};
}
}  // namespace backend::tumexam