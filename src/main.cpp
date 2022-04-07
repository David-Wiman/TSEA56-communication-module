#include <iostream>
#include <boost/asio.hpp>
#include <json.hpp>

#include "drivedata.h"
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

        DriveData drivedata{1,2,3,4,5,6,7};
        string return_msg = drivedata.format_json();
        connection.write(return_msg);
    }
    return 0;
}
