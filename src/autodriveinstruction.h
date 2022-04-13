#ifndef AUTODRIVEINSTRUCTION_H
#define AUTODRIVEINSTRUCTION_H

#include <nlohmann/json.hpp>

class AutoDriveInstruction {
public:
    AutoDriveInstruction();
    AutoDriveInstruction(const nlohmann::json& j);

private:

};

#endif  // AUTODRIVEINSTRUCTION_H
