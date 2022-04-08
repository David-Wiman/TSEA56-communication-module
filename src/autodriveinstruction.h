#ifndef AUTODRIVEINSTRUCTION_H
#define AUTODRIVEINSTRUCTION_H

#include <json.hpp>

class AutoDriveInstruction {
public:
    AutoDriveInstruction();
    AutoDriveInstruction(const nlohmann::json& j);

private:

};

#endif  // AUTODRIVEINSTRUCTION_H
