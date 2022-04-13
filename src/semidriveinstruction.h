#ifndef SEMIDRIVEINSTRUCTION_H
#define SEMIDRIVEINSTRUCTION_H

#include <nlohmann/json.hpp>

class SemiDriveInstruction {
public:
    SemiDriveInstruction();
    SemiDriveInstruction(const nlohmann::json& j);

private:

};

#endif  // SEMIDRIVEINSTRUCTION_H
