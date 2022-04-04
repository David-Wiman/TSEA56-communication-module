#include <iostream>

#include "drivedata.h"

DriveData::DriveData(int time, float throttle, float steering,
      float driven_distance, int obsticle_distance, int lateral_position, float angle)
:time{time}, throttle{throttle}, steering{steering}, driven_distance{driven_distance},
      obsticle_distance{obsticle_distance}, lateral_position{lateral_position}, angle{angle} {
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
