#include "connection_data.h"
#include "raspi_common.h"

#include <nlohmann/json.hpp>
#include <string>
#include <sstream>

using json = nlohmann::json;
using namespace std;


ManualDriveInstruction::ManualDriveInstruction(const json& j) {
    throttle = *j["ManualDriveInstruction"].find("throttle");
    steering = *j["ManualDriveInstruction"].find("steering");
}

steer_data_t ManualDriveInstruction::as_steer_data() const {
    return steer_data_t{throttle, steering};
}


SemiDriveInstruction::SemiDriveInstruction(const json& j) {
    direction = *j["SemiDriveInstruction"].find("direction");
    id = *j["SemiDriveInstruction"].find("id");
}

drive_instruction_t SemiDriveInstruction::as_drive_instruction() const {
    drive_instruction_t d{
        static_cast<instruction::InstructionNumber>(direction),
        id
    };
    return d;
}

ostream& operator<<(ostream &os, SemiDriveInstruction const &semi_drive_instruction) {
    const array<string, 3> instruction_names{"Left", "Forward", "Right"};

    os << "Instruction: " << instruction_names.at(semi_drive_instruction.direction)
       << ", ID: " << semi_drive_instruction.id;
    return os;
}


AutoDriveInstruction::AutoDriveInstruction(const json& j) {}


DriveData::DriveData(int elasped_time, steer_data_t const &steer_data,
                     sensor_data_t const &sensor_data, int lateral_position, int angle)
: elapsed_time{elasped_time}, throttle{steer_data.gas},
  steering{steer_data.steer_angle}, obstacle_distance{sensor_data.obstacle_distance},
  driving_distance{sensor_data.driving_distance}, speed{sensor_data.speed},
  lateral_position{lateral_position}, angle{angle} {}

std::string DriveData::format_json() {
    std::ostringstream oss;
    oss << "{\"DriveData\": {\"elapsed_time\": " << elapsed_time
        << ", \"throttle\": " << throttle
        << ", \"steering\": " << steering
        << ", \"obstacle_distance\": " << obstacle_distance
        << ", \"driving_distance\": " << driving_distance
        << ", \"speed\": " << speed
        << ", \"lateral_position\": " << lateral_position
        << ", \"angle\": " << angle << "}}" ;

    return oss.str();
}


ParameterConfiguration::ParameterConfiguration()
: steering_kp{0}, steering_kd{0}, speed_kp{0}, speed_ki{0} {}

ParameterConfiguration::ParameterConfiguration(const json& j)
: steering_kp{0}, steering_kd{0}, speed_kp{0}, speed_ki{0} {
    steering_kp = *j["ParameterConfiguration"].find("steering_kp");
    steering_kd = *j["ParameterConfiguration"].find("steering_kd");
    speed_kp = *j["ParameterConfiguration"].find("speed_kp");
    speed_ki = *j["ParameterConfiguration"].find("speed_ki");
}

