#include "drivedata.h"
#include "connection.h"
#include "parameterconfiguration.h"
#include "manualdriveinstruction.h"
#include "semidriveinstruction.h"
#include "drivedata.h"
#include "log.h"
#include "communication_module.h"
#include "image_processing.h"
#include "control_center.h"
#include "raspi_common.h"

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
using steady_clock = chrono::steady_clock;

int main() {

    // Initiate
    Logger::init();
    i2c_init();
    Connection connection{1234};

    // Connection recieved, start clock
    steady_clock::time_point start_time = steady_clock::now();

    ImageProcessing image_processor{"image-processing-module/", false};
    CommunicationModule com{5};
    ControlCenter control_center{};

    sensor_data_t sensor_data{};
    image_proc_t image_data{};
    
    int elapsed_time{0};
    int16_t throttle{0};
    int16_t steering{0};

    while (true) {

        if (connection.emergency_recieved()) {
            cout << "Emergency stop recieved!" << endl;
            break; // Kill car
        }

        if (com.get_sensor_data(sensor_data)) {
            // Error
        }

        if (connection.new_manual_instruction()) {
            ManualDriveInstruction instruction = connection.get_manual_drive_instruction();
            throttle = instruction.get_throttle();
            steering = instruction.get_steering();
            com.send_manual_instruction(throttle, steering);
        } else if (connection.new_semi_instruction()) {
            SemiDriveInstruction instruction = connection.get_semi_drive_instruction();
            stringstream ss{};
            ss << instruction;
            Logger::log(INFO, __FILE__, "New (semi-auto) instruction", ss.str());

            control_center.add_drive_instruction(instruction.get_drive_instruction());
        } else {
            //cout << "No new instruction" << endl;
        }

        if (connection.has_lost_connection()) {
            Logger::log(ERROR, __FILE__, "Main", "Lost connection to user interface, exit.");
            break;
        }


        image_data = image_processor.process_next_frame();

        reference_t ref = control_center(sensor_data, image_data);
        string finished_instruction_id = control_center.get_finished_instruction_id();
        if (finished_instruction_id != "") {
            Logger::log(INFO, __FILE__, "Finished instruction: ", finished_instruction_id);
        }
        
        // Calculate time in seconds since client connection (start)
        auto time_diff = steady_clock::now() - start_time;
        elapsed_time = chrono::duration_cast<chrono::seconds>(time_diff).count();
        
        DriveData drivedata = DriveData(elapsed_time, throttle, steering,
                                        sensor_data, image_data.lateral_position,
                                        ref.angle);
        connection.write(drivedata.format_json());

        com.throttle();
    }

    return 0;
}

