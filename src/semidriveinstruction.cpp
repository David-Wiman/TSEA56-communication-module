#include <nlohmann/json.hpp>
#include <iostream>

#include "semidriveinstruction.h"

using namespace std;
using json = nlohmann::json;

SemiDriveInstruction::SemiDriveInstruction(): instruction{} {}

SemiDriveInstruction::SemiDriveInstruction(const json& j): instruction{} {
    int direction = *j["SemiDriveInstruction"].find("direction");
    string id = *j["SemiDriveInstruction"].find("id");
    instruction.number = static_cast<instruction::InstructionNumber>(direction);
    instruction.id = id;
}

drive_instruction_t SemiDriveInstruction::get_drive_instruction() const {
    return instruction;
}

int SemiDriveInstruction::get_direction() const {
    return instruction.number;
}

std::string SemiDriveInstruction::get_id() const {
    return instruction.id;
}

ostream& operator<<(ostream &os, SemiDriveInstruction const &semi_drive_instruction) {
    const array<string, 3> instruction_names{"Left", "Forward", "Right"};
    drive_instruction_t instruction = semi_drive_instruction.get_drive_instruction();

    os << "Instruction: " << instruction_names.at(instruction.number) 
        << ", ID: " << instruction.id;
    return os;
}

