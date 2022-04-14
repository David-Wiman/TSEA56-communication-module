#include <nlohmann/json.hpp>

#include "manualdriveinstruction.h"

using json = nlohmann::json;

ManualDriveInstruction::ManualDriveInstruction()
:throttle{0}, steering{0} {}

ManualDriveInstruction::ManualDriveInstruction(const json& j)
:throttle{0}, steering{0} {
    throttle = *j["ManualDriveInstruction"].find("throttle");
    steering = *j["ManualDriveInstruction"].find("steering");
}

int ManualDriveInstruction::get_throttle() {
    return throttle;
}

int ManualDriveInstruction::get_steering() {
    return steering;
}