#ifndef SEMIDRIVEINSTRUCTION_H
#define SEMIDRIVEINSTRUCTION_H

#include <string>
#include <nlohmann/json.hpp>

class SemiDriveInstruction {
public:
    SemiDriveInstruction();
    SemiDriveInstruction(const nlohmann::json& j);

    /* Available directions the car can drive in the autonomous modes */
    enum DIRECTION {LEFT, FORWARD, RIGHT};

    /* Returns direction, 0->left, 1->forward and 2->right */
    int get_direction();

    std::string& get_id();

private:
    int direction; // 0->left, 1->fwrd, 2->right
    std::string id;
};

#endif  // SEMIDRIVEINSTRUCTION_H
