#include "Student.hpp"
#include <string>

namespace backend::tumexam {
Student Student::from_json(const nlohmann::json& j) {
    if (!j.contains("erid")) {
        throw std::runtime_error("Failed to parse Student. 'erid' field missing.");
    }
    std::string erid;
    j.at("erid").get_to(erid);

    if (!j.contains("srid")) {
        throw std::runtime_error("Failed to parse Student. 'srid' field missing.");
    }
    std::string srid;
    j.at("srid").get_to(srid);

    if (!j.contains("matrikel")) {
        throw std::runtime_error("Failed to parse Student. 'matrikel' field missing.");
    }
    std::string matrikel;
    j.at("matrikel").get_to(matrikel);

    if (!j.contains("lastname")) {
        throw std::runtime_error("Failed to parse Student. 'lastname' field missing.");
    }
    std::string lastname;
    j.at("lastname").get_to(lastname);

    if (!j.contains("firstname")) {
        throw std::runtime_error("Failed to parse Student. 'firstname' field missing.");
    }
    std::string firstname;
    j.at("firstname").get_to(firstname);

    if (!j.contains("flag")) {
        throw std::runtime_error("Failed to parse Student. 'flag' field missing.");
    }
    std::string flags;
    j.at("flag").get_to(flags);

    return Student{
        std::move(erid),
        std::move(srid),
        std::move(matrikel),
        std::move(lastname),
        std::move(firstname),
        std::move(flags)};
}
}  // namespace backend::tumexam