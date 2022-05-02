#include "drivedata.h"

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
