#ifndef PARAMETERCONFIGURATION_H
#define PARAMETERCONFIGURATION_H

#include <nlohmann/json.hpp>

class ParameterConfiguration {
public:
    ParameterConfiguration();
    ParameterConfiguration(const nlohmann::json& j);

    int get_steering_kp();
    int get_steering_kd();
    int get_speed_kp();
    int get_speed_ki();

private:
    int steering_kp;
    int steering_kd;
    int speed_kp;
    int speed_ki;
};

#endif  // PARAMETERCONFIGURATION_H
