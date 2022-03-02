#include "TUMExamHelper.hpp"
#include "backend/tumexam/CorrectionStatus.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "cpr/cprtypes.h"
#include "spdlog/spdlog.h"
#include <logger/Logger.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
#include <cpr/cpr.h>

namespace backend::tumexam {
std::optional<Submissions> get_submission_status(const Credentials& credentials) {
    cpr::Url url = credentials.base_url + "/api/exam/" + credentials.exam + "/submission/status";
    cpr::Response r = cpr::Get(url, cpr::Cookies{{"session", credentials.session}, {"token", credentials.token}});
    if (r.status_code == 200) {
        try {
            const nlohmann::json j = nlohmann::json::parse(r.text);
            return std::make_optional<Submissions>(Submissions::from_json(j));

        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to parse submission status with: {}", e.what());
        }
    } else {
        SPDLOG_ERROR("Failed to request submission status with: {} {}", r.status_code, r.error.message);
    }
    return std::nullopt;
}

std::vector<std::shared_ptr<CorrectionStatus>> get_correction_status(const Credentials& credentials, bool subproblems) {
    cpr::Url url = credentials.base_url + "/api/exam/" + credentials.exam + "/correction/status" + (subproblems ? "?per_subproblem=true" : "");
    cpr::Response r = cpr::Get(url, cpr::Cookies{{"session", credentials.session}, {"token", credentials.token}});
    if (r.status_code == 200) {
        try {
            const nlohmann::json j = nlohmann::json::parse(r.text);
            nlohmann::json::array_t status_array = j;
            std::vector<std::shared_ptr<CorrectionStatus>> status;
            for (const nlohmann::json& jStatus : status_array) {
                status.emplace_back(std::make_shared<CorrectionStatus>(CorrectionStatus::from_json(jStatus)));
            }
            return status;

        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to parse submission status with: {}", e.what());
        }
    } else {
        SPDLOG_ERROR("Failed to request submission status with: {} {}", r.status_code, r.error.message);
    }
    return std::vector<std::shared_ptr<CorrectionStatus>>{};
}
}  // namespace backend::tumexam
