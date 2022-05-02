#include "drivedata.h"
#include "connection.h"
#include "manualdriveinstruction.h"
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

int main() {

    // Initiate
    Logger::init();
    i2c_init();
    Connection connection{1234};
    ImageProcessing image_processor{"image-processing-module/", false};
    CommunicationModule com{5, 100, 100};
    ControlCenter control_center{};

    sensor_data_t sensor_data{};
    steer_data_t steer_data{};
    image_proc_t image_data{};

    while (true) {

        if (connection.emergency_recieved()) {
            cout << "Emergency stop recieved!" << endl;
            break; // Kill car
        }

        com.update_sensor_data(sensor_data);

        com.update_steer_data(steer_data);

        if (connection.new_manual_instruction()) {
            ManualDriveInstruction instruction = connection.get_manual_drive_instruction();
            int16_t throttle = instruction.get_throttle();
            int16_t steering = instruction.get_steering();
            com.enqueue_manual_instruction(throttle, steering);
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
        DriveData drivedata = DriveData(0, 0, 0, sensor_data, image_data.lateral_position, ref.angle);
        connection.write(drivedata.format_json());

        com.throttle();
    }

    return 0;
}

