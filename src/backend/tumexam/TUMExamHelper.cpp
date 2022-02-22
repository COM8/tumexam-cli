#include "TUMExamHelper.hpp"
#include "cpr/cprtypes.h"
#include "spdlog/spdlog.h"
#include <logger/Logger.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <cpr/cpr.h>

namespace backend::tumexam {
std::optional<Submissions> get_submission_status(const std::string& base_url, const std::string& exam, const std::string& session, const std::string& token) {
    cpr::Url url = base_url + "/api/exam/" + exam + "/submission/status";
    cpr::Response r = cpr::Get(url, cpr::Cookies{{"session", session}, {"token", token}});
    if (r.status_code == 200) {
        try {
            const nlohmann::json j = nlohmann::json::parse(r.text);
            return Submissions::from_json(j);

        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to parse submission status with: {}", e.what());
        }
    } else {
        SPDLOG_ERROR("Failed to request submission status with: {} {}", r.status_code, r.error.message);
    }
    return std::nullopt;
}
}  // namespace backend::tumexam
