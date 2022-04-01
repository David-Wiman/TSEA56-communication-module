#ifndef MANUALDRIVEINSTRUCTION_H
#define MANUALDRIVEINSTRUCTION_H

#include <string>
#include <boost/asio.hpp>
#include <json.hpp>

class ManualDriveInstruction {
public:
    ManualDriveInstruction(const nlohmann::json& j);
    
    float get_throttle();
    float get_steering();

private:
    float throttle;
    float steering;
};

#endif  // MANUALDRIVEINSTRUCTION_H
