#include "ExamSubmissionDetails.hpp"
#include "ParseUtils.hpp"

namespace backend::tumexam {
ExamSubmissionDetails ExamSubmissionDetails::from_json(const nlohmann::json& j) {
    return ExamSubmissionDetails{
        parse_time(j, "start", "ExamSubmissionDetails"),
        parse_time(j, "end", "ExamSubmissionDetails"),
        parse_time(j, "submissionEnd", "ExamSubmissionDetails"),
        parse_time(j, "uploadEnd", "ExamSubmissionDetails")};
}
}  // namespace backend::tumexam