#include <iostream>
#include <boost/asio.hpp>
#include <json.hpp>

#include "manualdriveinstruction.h"

using namespace boost::asio;
using json = nlohmann::json;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;


ManualDriveInstruction::ManualDriveInstruction(const json& j)
:throttle{0}, steering{0} { 
    throttle = *j["ManualDriveInstruction"].find("throttle");
    steering = *j["ManualDriveInstruction"].find("steering");
}


float ManualDriveInstruction::get_throttle() {
      return throttle;
}

float ManualDriveInstruction::get_steering() {
      return steering;
}
