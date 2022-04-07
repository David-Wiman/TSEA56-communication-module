#include <iostream>
#include <boost/asio.hpp>
#include <json.hpp>

#include "connection.h"
#include "manualdriveinstruction.h"
#include "drivedata.h"
#include "log.h"

using namespace std;
using json = nlohmann::json;

bool exists(const json& j, const string& key) {
    return j.find(key) != j.end();
}

int main() {
    Logger log{"log.txt"}; //create a log
    
    Connection connection{1234};
    
    while (true) {
        string msg{};
        try {
            msg = connection.read();
        } catch (boost::exception&) {
            cout << "Lost connection" << endl;
            break;
        }
                
        json j{};
        try {
            j = json::parse(msg);
        } catch (std::invalid_argument&) {
            cout << "Invalid argument" << endl;
        }
        
        if (exists(j, "ManualDriveInstruction")) {
            ManualDriveInstruction inst1(j);
            cout << "Throttle: " << inst1.get_throttle() << endl;
            cout << "Steering: " << inst1.get_steering() << endl;
            
            log.log(INFO, "User interface", "Throttle", inst1.get_throttle());
            log.log(DEBUG, "User interface", "Steering", inst1.get_steering());
        }
            
        
        connection.write("Acknowledge");
    }
    
    return 0;
}
    
   
