#ifndef MANUALDRIVEINSTRUCTION_H
#define MANUALDRIVEINSTRUCTION_H

#include <json.hpp>

class ManualDriveInstruction {
public:
    ManualDriveInstruction();
    ManualDriveInstruction(const nlohmann::json& j);

    int get_throttle();
    int get_steering();

private:
    int throttle;
    int steering;
};

#endif  // MANUALDRIVEINSTRUCTION_H
