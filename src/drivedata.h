#ifndef DRIVEDATA_H
#define DRIVEDATA_H

class DriveData {
public:
    DriveData(int time, float throttle, float steering,
          float driven_distance, int obsticle_distance, int lateral_position, float angle);

    int get_time();
    float get_throttle();
    float get_steering();
    float get_driven_distance();
    int get_obsticle_distance();
    int get_lateral_position();
    float get_angle();

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
