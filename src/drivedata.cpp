#include <iostream>
#include <boost/asio.hpp>
#include <json.hpp>

#include "drivedata.h"

using namespace boost::asio;
using json = nlohmann::json;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;


DriveData::DriveData(const json& j)
:time{0}, throttle{0}, steering{0}, driven_distance{0},
      obsticle_distance{0}, lateral_position{0}, angle{0} {   
      time = *j["DriveData"].find("time");
      throttle = *j["DriveData"].find("throttle");
      steering = *j["DriveData"].find("steering");
      driven_distance = *j["DriveData"].find("driven_distance");
      obsticle_distance = *j["DriveData"].find("obsicle_distance");
      lateral_position = *j["DriveData"].find("lateral_position");
      angle = *j["DriveData"].find("angle");
}


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

int DriveData::get_obsticle_distance() {
      return obsticle_distance;
}

int DriveData::get_lateral_position() {
      return lateral_position;
}

float DriveData::get_angle() {
      return angle;
}

