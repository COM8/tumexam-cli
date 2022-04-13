#include "TUMExamHelper.hpp"
#include "backend/tumexam/CorrectionPass.hpp"
#include "backend/tumexam/CorrectionStatus.hpp"
#include "backend/tumexam/Credentials.hpp"
#include "backend/tumexam/Submissions.hpp"
#include "cpr/api.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/payload.h"
#include "cpr/session.h"
#include "cpr/util.h"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <logger/Logger.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <regex>
#include <string>
#include <utility>
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

std::optional<Feedbacks> get_feedbacks(const Credentials& credentials) {
    cpr::Url url = credentials.base_url + "/api/exam/" + credentials.exam + "/review/get_erid";
    cpr::Response r = cpr::Get(url, cpr::Cookies{{"session", credentials.session}, {"token", credentials.token}});
    if (r.status_code == 200) {
        try {
            const nlohmann::json j = nlohmann::json::parse(r.text);
            return std::make_optional<Feedbacks>(Feedbacks::from_json(j));
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to parse feedback with: {}", e.what());
        }
    } else {
        SPDLOG_ERROR("Failed to request feedback with: {} {}", r.status_code, r.error.message);
    }
    return std::nullopt;
}

std::string extract_csrf_token(const std::string& data) {
    static const std::regex csrf_token_regex(R"lit([\W|\w]*name="csrf_token"\s+value="(.*)"[\W|\w]*)lit");
    std::smatch match;
    if (!std::regex_match(data, match, csrf_token_regex)) {
        return "";
    }
    if (match.size() != 2) {
        return "";
    }
    std::ssub_match token_match = match[1];
    return token_match.str();
}

std::string extract_relay_state(const std::string& data) {
    static const std::regex relay_state_regex(R"lit([\W|\w]*name="RelayState"\s+value=\"(.+)"[\W|\w]*)lit");
    std::smatch match;
    if (!std::regex_match(data, match, relay_state_regex)) {
        return "";
    }
    if (match.size() != 2) {
        return "";
    }
    std::ssub_match relay_state_match = match[1];
    std::string relay_state = relay_state_match.str();
    const std::string old = "ss&#x3a;mem&#x3a;";
    return "ss:mem:" + relay_state.substr(old.size());
}

std::string extract_saml_response(const std::string& data) {
    static const std::regex saml_response_regex(R"lit([\W|\w]*name="SAMLResponse"\s+value=\"(.+)"[\W|\w]*)lit");
    std::smatch match;
    if (!std::regex_match(data, match, saml_response_regex)) {
        return "";
    }
    if (match.size() != 2) {
        return "";
    }
    std::ssub_match saml_response_match = match[1];
    return saml_response_match.str();
}


std::shared_ptr<Credentials> login(const std::string& instance, const std::string& username, const std::string& password) {
    const std::string base_url = "https://" + instance + ".hq.tumexam.de";
    cpr::Response r = cpr::Get(cpr::Url{base_url});
    if (r.status_code != 200) {
        SPDLOG_ERROR("Requesting the login page for '{}' failed with: {} {}", instance, r.status_code, r.error.message);
        return nullptr;
    }
    const std::string csrf_token = extract_csrf_token(r.text);
    if (csrf_token.empty()) {
        SPDLOG_ERROR("Login failed. No csrf token found in: {}", r.text);
        return nullptr;
    }

    cpr::Payload payload{
        {"csrf_token", csrf_token},
        {"j_username", username},
        {"j_password", password},
        {"donotcache", "1"},
        {"_eventId_proceed", ""},
    };
    cpr::Cookies cookies = r.cookies;
    r = cpr::Post(cpr::Url{"https://login.tum.de/idp/profile/SAML2/Redirect/SSO?execution=e1s1"}, std::move(payload), std::move(cookies));

    if (r.status_code != 200) {
        SPDLOG_ERROR("Failed to obtain 'shib_idp_session' for '{}' with: {} {}", instance, r.status_code, r.error.message);
        return nullptr;
    }

    const std::string shib_idp_session = r.cookies["shib_idp_session"];
    if(shib_idp_session.empty()) {
        SPDLOG_ERROR("Failed to obtain 'shib_idp_session' cookie for '{}' with: Cookie missing in response.", instance);
        return nullptr;
    }

    const std::string relay_state = extract_relay_state(r.text);
    if(relay_state.empty()) {
        SPDLOG_ERROR("Failed to obtain 'RelayState' for '{}' with: Invalid response.", instance);
        return nullptr;
    }
    const std::string saml_response = extract_saml_response(r.text);
    if(saml_response.empty()) {
        SPDLOG_ERROR("Failed to obtain 'SAMLResponse' for '{}' with: Invalid response.", instance);
        return nullptr;
    }

    cpr::Session session;
    session.SetUrl(cpr::Url("https://hq.tumexam.de/Shibboleth.sso/SAML2/POST"));
    session.SetPayload(cpr::Payload{{"RelayState", relay_state},
    {"SAMLResponse", saml_response}});
    r = session.Post();
    if (r.status_code != 302) {
        SPDLOG_ERROR("Failed to obtain 'shibsession' cookie for '{}' with: {} {}", instance, r.status_code, r.error.message);
        return nullptr;
    }

    std::string shibsession_cookie_name;
    std::string shibsession_cookie_value;
    for (const auto& cookie : r.cookies) {
        // NOLINTNEXTLINE (abseil-string-find-str-contains)
        if(cookie.first.find("_shibsession_") != std::string::npos) {
            shibsession_cookie_name = cookie.first;
            shibsession_cookie_value = cookie.second;
            break;
        }
    }
    if(shibsession_cookie_name.empty() || shibsession_cookie_value.empty()) {
        SPDLOG_ERROR("Failed to obtain '_shibsession_' cookie for '{}' with: Cookie missing in response.", instance);
        return nullptr;
    }

    session.SetUrl(cpr::Url("https://hq.tumexam.de/login/" + instance + "/"));
    session.SetCookies(r.cookies);
    r = session.Get();
    if (r.status_code != 200) {
        SPDLOG_ERROR("Failed to confirm login for '{}' with: {} {}", instance, r.status_code, r.error.message);
        return nullptr;
    }

    const std::string token_cookie = r.cookies["token"];
    if(token_cookie.empty()) {
        SPDLOG_ERROR("Failed to obtain 'token' cookie for '{}' with: Cookie missing in response.", instance);
        return nullptr;
    }

    const std::string session_cookie = r.cookies["session"];
    if(session_cookie.empty()) {
        SPDLOG_ERROR("Failed to obtain 'session' cookie for '{}' with: Cookie missing in response.", instance);
        return nullptr;
    }

    SPDLOG_INFO("Login for {} was successful!", instance);
    SPDLOG_DEBUG("Session: {}, Token: {}", session_cookie, token_cookie);
    return std::make_shared<backend::tumexam::Credentials>(
        base_url,
        "",
        session_cookie,
        token_cookie);
}

int parse_students_page(const nlohmann::json& j, std::vector<Student> result) {
    if (!j.contains("max_page")) {
        throw std::runtime_error("Failed to parse students. 'max_page' field missing.");
    }
    int pages = 0;
    j.at("max_page").get_to(pages);

    if (!j.contains("results")) {
        throw std::runtime_error("Failed to parse students. 'results' field missing.");
    }
    nlohmann::json::array_t students_array;
    j.at("results").get_to(students_array);
    std::vector<std::shared_ptr<SubmissionStudent>> students;
    for (const nlohmann::json& jStudent : students_array) {
        result.emplace_back(Student::from_json(jStudent));
    }
    return pages;
}

std::vector<Student> get_students(const Credentials& credentials) {
    std::vector<Student> result;

    cpr::Session session;
    session.SetCookies(cpr::Cookies{{"session", credentials.session}, {"token", credentials.token}});
    int pages = 2;
    for (int page = 1; page <= pages; page++)
    {
        session.SetUrl(cpr::Url{credentials.base_url + "/api/exam/" + credentials.exam + "/grade?page=" + std::to_string(page)});
        cpr::Response r = session.Get();
        if (r.status_code == 200) {
        try {
            const nlohmann::json j = nlohmann::json::parse(r.text);
            pages = parse_students_page(j, result);
            SPDLOG_DEBUG("Downloaded students page {} out of {}.", page, pages);
            continue;
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to parse students on page {} with: {}", page, e.what());
        }
        } else {
            SPDLOG_ERROR("Failed to request students page {} with: {} {}", page, r.status_code, r.error.message);
        }
        result.clear();
        break;
    }
    return result;
}
}  // namespace backend::tumexam
