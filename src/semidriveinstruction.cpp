#include <nlohmann/json.hpp>

#include "semidriveinstruction.h"

using namespace std;
using json = nlohmann::json;

SemiDriveInstruction::SemiDriveInstruction()
:direction{}, id{} {}

SemiDriveInstruction::SemiDriveInstruction(const json& j)
:direction{}, id{} {
    direction = *j["SemiDriveInstruction"].find("direction");
    id = *j["SemiDriveInstruction"].find("id");
}

int SemiDriveInstruction::get_direction() {
    return direction;
}

string& SemiDriveInstruction::get_id() {
    return id;
}

