#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace backend::tumexam {
class Student {
 public:
    std::string erid;
    std::string srid;
    std::string matrikel;
    std::string lastname;
    std::string firstname;
    std::string flags;

    static Student from_json(const nlohmann::json& j);
};
}  // namespace backend::tumexam