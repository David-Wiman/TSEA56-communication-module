#include <nlohmann/json.hpp>

#include "parameterconfiguration.h"

using json = nlohmann::json;

ParameterConfiguration::ParameterConfiguration()
: steering_kp{0}, steering_kd{0}, speed_kp{0}, speed_ki{0} {}

ParameterConfiguration::ParameterConfiguration(const json& j)
: steering_kp{0}, steering_kd{0}, speed_kp{0}, speed_ki{0} {
    steering_kp = *j["ParameterConfiguration"].find("steering_kp");
    steering_kd = *j["ParameterConfiguration"].find("steering_kd");
    speed_kp = *j["ParameterConfiguration"].find("speed_kp");
    speed_ki = *j["ParameterConfiguration"].find("speed_ki");
}

int ParameterConfiguration::get_steering_kp() {
    return steering_kp;
}

int ParameterConfiguration::get_steering_kd() {
    return steering_kd;
}

int ParameterConfiguration::get_speed_kp() {
    return speed_kp;
}

int ParameterConfiguration::get_speed_ki() {
    return speed_ki;
}