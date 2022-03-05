#include "TUMExamHelper.hpp"
#include "backend/tumexam/CorrectionPass.hpp"
#include "backend/tumexam/CorrectionStatus.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "cpr/cprtypes.h"
#include "spdlog/spdlog.h"
#include <logger/Logger.hpp>
#include <memory>
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
    cpr::Url url = credentials.base_url + "/api/exam/" + credentials.exam + "/correction/status?per_subproblem=true";
    cpr::Response r = cpr::Get(url, cpr::Cookies{{"session", credentials.session}, {"token", credentials.token}});
    if (r.status_code == 200) {
        try {
            const nlohmann::json j = nlohmann::json::parse(r.text);
            nlohmann::json::array_t status_array = j;
            std::vector<std::shared_ptr<CorrectionStatus>> correctionStatus;
            for (const nlohmann::json& jStatus : status_array) {
                CorrectionStatus newStatus = CorrectionStatus::from_json(jStatus);
                CorrectionPass pass = CorrectionPass::from_json(jStatus);

                // Check if the problem/subproblem is already known:
                std::shared_ptr<CorrectionStatus> status{nullptr};
                for (std::shared_ptr<CorrectionStatus>& s : correctionStatus) {
                    if ((subproblems && *s == newStatus) || (!subproblems && s->problem == newStatus.problem)) {
                        status = s;
                        break;
                    }
                }

                // New problem/subproblem:
                if (!status) {
                    correctionStatus.push_back(std::make_shared<CorrectionStatus>(newStatus));
                    status = correctionStatus.back();
                }

                // Update correction pass:
                if (pass.pass == 1) {
                    if (subproblems) {
                        status->pass1 = pass;
                    } else {
                        if (status->pass1) {
                            *(status->pass1) += pass;
                        } else {
                            status->pass1 = pass;
                        }
                    }
                } else if (pass.pass == 2) {
                    if (subproblems) {
                        status->pass2 = pass;
                    } else {
                        if (status->pass2) {
                            *(status->pass2) += pass;
                        } else {
                            status->pass2 = pass;
                        }
                    }
                } else {
                    SPDLOG_ERROR("Unknown correction pass '{}' found.", pass.pass);
                }
            }

            if (!subproblems) {
                for (std::shared_ptr<CorrectionStatus>& s : correctionStatus) {
                    s->subproblem = std::nullopt;
                    if (s->pass1) {
                        s->pass1->corrected /= s->pass1->subproblem_count;
                    }

                    if (s->pass2) {
                        s->pass2->corrected /= s->pass2->subproblem_count;
                        s->subproblem = std::nullopt;
                    }
                }
            }
            return correctionStatus;

        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to parse submission status with: {}", e.what());
        }
    } else {
        SPDLOG_ERROR("Failed to request submission status with: {} {}", r.status_code, r.error.message);
    }
    return std::vector<std::shared_ptr<CorrectionStatus>>{};
}
}  // namespace backend::tumexam
