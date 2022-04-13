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

class DriveData {
public:
    DriveData(int time, float throttle, float steering, float driven_distance,
              int obstacle_distance, int lateral_position, float angle);

    int get_time();
    float get_throttle();
    float get_steering();
    float get_driven_distance();
    int get_obstacle_distance();
    int get_lateral_position();
    float get_angle();
    void update_sensor_data(sensor_data_t const &sensor_data);

    std::string format_json();

private:
    int time;
    float throttle;
    float steering;
    float driven_distance;
    int obstacle_distance;
    int lateral_position;
    float angle;
};

#endif  // DRIVEDATA_H
