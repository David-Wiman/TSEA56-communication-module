#ifndef SEMIDRIVEINSTRUCTION_H
#define SEMIDRIVEINSTRUCTION_H

#include <string>
#include <nlohmann/json.hpp>

#include "raspi_common.h"

class SemiDriveInstruction {
public:
    SemiDriveInstruction();
    SemiDriveInstruction(const nlohmann::json& j);

    drive_instruction_t get_drive_instruction() const;
    int get_direction() const;
    std::string get_id() const;

private:
    drive_instruction_t instruction;
};

std::ostream& operator<<(std::ostream &os, SemiDriveInstruction const &semi_drive_instruction);

#endif  // SEMIDRIVEINSTRUCTION_H
