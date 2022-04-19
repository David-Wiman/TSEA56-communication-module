#ifndef DRIVEDATA_H
#define DRIVEDATA_H

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef struct sensor_data_type {
    int obstacle_distance = 0;
    int driving_distance = 0;
    int speed = 0;
} sensor_data_t;

typedef struct camera_data_type {
    int lateral_position = 0;
    int angle = 0;
} camera_data_t;

class DriveData {
public:
    DriveData(int elasped_time, int throttle, int steering,
              sensor_data_t sensor_data, camera_data_t camera_data);

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
