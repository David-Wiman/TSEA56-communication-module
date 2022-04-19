#include "drivedata.h"

DriveData::DriveData(int elasped_time, int throttle, int steering,
                    sensor_data_t sensor_data, camera_data_t camera_data)
: elapsed_time{elasped_time}, throttle{throttle}, steering{steering}, 
  obstacle_distance{sensor_data.obstacle_distance},
  driving_distance{sensor_data.driving_distance}, speed{sensor_data.speed},
  lateral_position{camera_data.lateral_position}, angle{camera_data.angle} {}

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
    std::string msg = oss.str();
    return msg;
}
