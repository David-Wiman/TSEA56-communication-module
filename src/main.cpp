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

#define MAX_IMAGE_ERRORS 10

using namespace std;
using json = nlohmann::json;
using steady_clock = chrono::steady_clock;

int main() {

    // Initiate
    Logger::init(true);
    i2c_init();
    Connection connection{1234};

    // Connection recieved, start clock
    steady_clock::time_point start_time = steady_clock::now();

    ImageProcessing image_processor{"image-processing-module/", false};
    CommunicationModule com{};
    ControlCenter control_center{3, 3, 5, 2, 4};

    sensor_data_t sensor_data{};
    steer_data_t steer_data{};
    image_proc_t image_data{};
    control_t control_data{};

    drive_mode::DriveMode mode{drive_mode::manual};

    com.write_regulation_constants(100, 130, 2, 1, 90, 1630);

    int elapsed_time{0};
    int image_error_counter{0};
    unsigned program_counter{0};

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

        if (program_counter % 2)
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
        } else if (connection.new_auto_instruction()) {
            mode = drive_mode::full_auto;
            DriveMission missions = connection.get_drive_mission();
            control_center.set_drive_missions(missions.get_target_nodes());
            Logger::log(INFO, __FILE__, "New (full-auto) instruction", "");
        } else if (connection.new_map()) {
            json mapdata = connection.get_map();
            control_center.update_map(mapdata);
            Logger::log(INFO, __FILE__, "New map recieved", mapdata.dump());
        } else {
            // No new instruction
        }

        switch (mode) {
            case drive_mode::manual:
                com.read_steer_data(steer_data);
                break;

            case drive_mode::semi_auto:
                image_data = image_processor.get_next_image_data();
                if (image_data.status_code == 2) {
                    Logger::log(ERROR, __FILE__,
                        "Image processor error count", ++image_error_counter);
                } else {
                    image_error_counter = 0;
                    control_data = control_center(sensor_data, image_data);
                    com.write_auto_instruction(
                            control_data, min(sensor_data.speed, 1000)
                        );
                    if (control_center.finished_instruction()) {
                        string instruction_id = control_center.get_finished_instruction_id();
                        Logger::log(INFO, __FILE__, "Finished instruction: ", instruction_id);
                        connection.write_formated("InstructionId", instruction_id);
                    }
                }
                break;

            case drive_mode::full_auto:
                image_data = image_processor.get_next_image_data();
                if (image_data.status_code == 2) {
                    Logger::log(ERROR, __FILE__,
                        "Image processor error count", ++image_error_counter);
                } else {
                    image_error_counter = 0;
                    control_data = control_center(sensor_data, image_data);
                    com.write_auto_instruction(
                            control_data, min(sensor_data.speed, 1000)
                        );
                    if (control_center.finished_instruction()) {
                        string instruction_id = control_center.get_finished_instruction_id();
                        Logger::log(INFO, __FILE__, "Finished instruction: ", instruction_id);
                        connection.write_formated("Position", control_center.get_current_road_segment());
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
                control_data.lateral_position, control_data.angle);

        connection.write(drivedata.format_json());

        if (image_error_counter > MAX_IMAGE_ERRORS) {
            Logger::log(ERROR, __FILE__, "Image processor error", "Abort");
            break;
        }

        com.throttle(100);
        ++program_counter;
    }

    Logger::log(INFO, __FILE__, "Main", "Closing main");
    com.write_manual_instruction(0, 0);
    return 0;
}

