#include "connection.h"
#include "connection_data.h"
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
    CommunicationModule com{10};
    ControlCenter control_center{};

    sensor_data_t sensor_data{};
    steer_data_t steer_data{};
    image_proc_t image_data{};
    reference_t reference{};

    drive_mode::DriveMode mode{drive_mode::manual};

    com.write_regulation_constants(0, 0, 2, 2, 0, 0);

    int elapsed_time{0};

    while (true) {

        if (connection.emergency_recieved()) {
            cout << "Emergency stop recieved!" << endl;
            break; // Kill car
        }

        if (connection.has_lost_connection()) {
            Logger::log(ERROR, __FILE__, "Main", "Lost connection to user interface, exit.");
            break;
        }

        if (connection.new_parameters()) {
            ParameterConfiguration params = connection.get_parameter_configuration();
            stringstream ss{};
            ss << params;
            Logger::log(INFO, __FILE__, "New regulation parameters", ss.str());
            com.write_regulation_constants(params);
        }

        com.read_sensor_data(sensor_data);

        if (connection.new_manual_instruction()) {
            mode = drive_mode::manual;
            ManualDriveInstruction instruction = connection.get_manual_drive_instruction();
            steer_data = instruction.as_steer_data();
            com.write_manual_instruction(steer_data.gas, steer_data.steer_angle);
        } else if (connection.new_semi_instruction()) {
            mode = drive_mode::semi_auto;
            SemiDriveInstruction instruction = connection.get_semi_drive_instruction();
            stringstream ss{};
            ss << instruction;
            Logger::log(INFO, __FILE__, "New (semi-auto) instruction", ss.str());

            control_center.add_drive_instruction(instruction.as_drive_instruction());
        } else {
            //cout << "No new instruction" << endl;
        }

        switch (mode) {
            case drive_mode::manual:
                com.read_steer_data(steer_data);
                break;

            case drive_mode::semi_auto:
                {
                    com.read_steer_data(steer_data);
                    image_data = image_processor.process_next_frame();
                    reference = control_center(sensor_data, image_data);
                    com.write_auto_instruction(reference, sensor_data.speed, image_data.lateral_position);
                    string finished_instruction_id = control_center.get_finished_instruction_id();
                    if (finished_instruction_id != "") {
                        Logger::log(INFO, __FILE__, "Finished instruction: ", finished_instruction_id);
                        connection.send_instruction_id(finished_instruction_id);
                    }
                }
                break;

            default:
                Logger::log(ERROR, __FILE__, "Main", "Unexpected drive mode");
        }

        // Calculate time in seconds since client connection (start)
        auto time_diff = steady_clock::now() - start_time;
        elapsed_time = chrono::duration_cast<chrono::milliseconds>(time_diff).count();

        DriveData drivedata = DriveData(
                elapsed_time, steer_data, sensor_data,
                image_data.lateral_position, reference.angle);

        connection.write(drivedata.format_json());

        com.throttle();
    }

    return 0;
}

