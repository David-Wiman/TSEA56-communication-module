#ifndef CONNECTION_DATA_H
#define CONNECTION_DATA_H

#include "raspi_common.h"

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>


class ManualDriveInstruction {
public:
    ManualDriveInstruction() {}
    ManualDriveInstruction(nlohmann::json const& j);

    steer_data_t as_steer_data() const;

    int throttle{0};
    int steering{0};
};


class SemiDriveInstruction {
public:
    SemiDriveInstruction() {}
    SemiDriveInstruction(const nlohmann::json& j);

    drive_instruction_t as_drive_instruction() const;

    int direction{0};
    std::string id{"unset id"};
};

std::ostream& operator<<(std::ostream &os, SemiDriveInstruction const &semi_drive_instruction);

class AutoDriveInstruction {
public:
    AutoDriveInstruction();
    AutoDriveInstruction(const nlohmann::json& j);
    std::list<std::string> get_target_nodes();

private:
    std::list<std::string> target_nodes{};
};

class DriveData {
public:
    DriveData(int elasped_time, steer_data_t const &steer_data,
              sensor_data_t const &sensor_data, int lateral_position, int angle);

    std::string format_json();

private:
    int elapsed_time;
    int throttle;
    int steering;
    int obstacle_distance;
    int driving_distance;
    int speed;
    int lateral_position;
    int angle;
};

class ParameterConfiguration {
public:
    ParameterConfiguration();
    ParameterConfiguration(const nlohmann::json& j);

    int steering_kp{0};
    int steering_kd{0};
    int speed_kp{0};
    int speed_ki{0};
    int turn_kp{0};
    int turn_kd{0};
};

std::ostream& operator<<(std::ostream &os, ParameterConfiguration const &param_config);

#endif  // CONNECTION_DATA_H
