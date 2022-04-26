#ifndef DRIVEDATA_H
#define DRIVEDATA_H

#include "raspi_common.h"

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class DriveData {
public:
    DriveData(int elasped_time, int throttle, int steering,
              sensor_data_t sensor_data, int lateral_position, int angle);

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

#endif  // DRIVEDATA_H
