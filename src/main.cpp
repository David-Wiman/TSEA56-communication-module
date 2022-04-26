#include "i2c_common.h"
#include "drivedata.h"
#include "connection.h"
#include "manualdriveinstruction.h"
#include "drivedata.h"
#include "log.h"
#include "communication_module.h"
#include "image_processing.h"
#include "control_center.h"

extern "C" {
    #include "i2c.h"
    #include "i2c_common.h"
}

#include <iostream>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

int main() {

    // Initiate
    Logger::init();
    i2c_init();
    Connection connection{1234};
    ImageProcessing image_processor{false};
    CommunicationModule com{5};
    ControlCenter control_center{};

    sensor_data_t sensor_data{};
    camera_data_t camera_data{};
    image_proc_t image_data{};

    while (true) {

        image_data = image_processor.process_next_frame();

        if (connection.emergency_recieved()) {
            cout << "Emergency stop recieved!" << endl;
            break; // Kill car
        }

        if (com.get_sensor_data(sensor_data)) {
            // Error
        }

        if (connection.new_manual_instruction()) {
            ManualDriveInstruction instruction = connection.get_manual_drive_instruction();
            int16_t throttle = instruction.get_throttle();
            int16_t steering = instruction.get_steering();
            com.send_manual_instruction(throttle, steering);
        } else if (connection.new_semi_instruction()) {
            SemiDriveInstruction instruction = connection.get_semi_drive_instruction();
            cout << "Recieved direction: " << instruction.get_direction() << endl;
            cout << "Recieved id: " << instruction.get_id() << endl; 

            // Process instruction here
        } else {
            //cout << "No new instruction" << endl;
        }

        if (connection.has_lost_connection()) {
            break;
        }
        DriveData drivedata = DriveData(0, 0, 0, sensor_data, camera_data);
        connection.write(drivedata.format_json());

        com.throttle();
    }

    return 0;
}

