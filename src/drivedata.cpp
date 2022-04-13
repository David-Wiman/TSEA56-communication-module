#include "drivedata.h"

DriveData::DriveData(
      int time, float throttle, float steering, float driven_distance, 
      int obstacle_distance, int lateral_position, float angle)
: time{time}, throttle{throttle}, steering{steering}, 
  driven_distance{driven_distance}, obstacle_distance{obstacle_distance}, 
  lateral_position{lateral_position}, angle{angle} {}

int DriveData::get_time() {
    return time;
}

float DriveData::get_throttle() {
    return throttle;
}

float DriveData::get_steering() {
    return steering;
}

float DriveData::get_driven_distance() {
    return driven_distance;
}

int DriveData::get_obstacle_distance() {
    return obstacle_distance;
}

int DriveData::get_lateral_position() {
    return lateral_position;
}

float DriveData::get_angle() {
    return angle;
}

std::string DriveData::format_json() {
    std::ostringstream oss;
    oss << "{\"DriveData\": {\"time\": " << time
        << ", \"throttle\": " << throttle
        << ", \"steering\": " << steering
        << ", \"driven_distance\": " << driven_distance
        << ", \"obstacle_distance\": " << obstacle_distance
        << ", \"lateral_position\": " << lateral_position
        << ", \"angle\": " << angle << "}}" ;
    std::string msg = oss.str();
    return msg;
}
