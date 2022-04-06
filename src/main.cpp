#include <iostream>
#include <boost/asio.hpp>
#include <json.hpp>

#include "connection.h"
#include "manualdriveinstruction.h"

using namespace std;
using json = nlohmann::json;

int main() {
    Connection connection{1234};
    
    while (true) {        
        connection.read();
        
        if (connection.new_manual_instruction()) {
            ManualDriveInstruction instruction = connection.get_manual_drive_instruction();
            cout << "Recieved throttle: " << instruction.get_throttle() << endl;
            cout << "Recieved steering: " << instruction.get_steering() << endl;
        }
        connection.write("Acknowledge");
    }
    return 0;
}
