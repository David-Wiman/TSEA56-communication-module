#ifndef DRIVEDATA_H
#define DRIVEDATA_H

#include <string>
#include <sstream>
#include <json.hpp>

using json = nlohmann::json;

class DriveData {
public:
    DriveData(int time, float throttle, float steering, float driven_distance,
              int obsticle_distance, int lateral_position, float angle);

    int get_time();
    float get_throttle();
    float get_steering();
    float get_driven_distance();
    int get_obsticle_distance();
    int get_lateral_position();
    float get_angle();

    std::string format_json();

private:
    int time;
    float throttle;
    float steering;
    float driven_distance;
    int obsticle_distance;
    int lateral_position;
    float angle;
};

#endif  // DRIVEDATA_H
