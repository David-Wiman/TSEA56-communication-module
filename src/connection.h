#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "raspi_common.h"
#include "parameterconfiguration.h"
#include "manualdriveinstruction.h"
#include "semidriveinstruction.h"
#include "autodriveinstruction.h"

using json = nlohmann::json;

/* A socket connection to the user interface. */
class Connection {
public:
    /* Establish a connection on specified port, not done until client responds */
    Connection(int port);
    ~Connection();

    /* Delete unused methods */
    Connection(const Connection&) = delete;
    Connection operator=(const Connection&) = delete;

    /* Reestablish connection, same operations as in constructor */
    void restart();

    /* Sends a string to the client */
    void write(const std::string& response);

    /* Sends a instruction id to client */
    void send_instruction_id(const std::string& id);

    bool has_lost_connection();

    /* Returns 'true' if user pressed emergency stop */
    bool emergency_recieved();

    /* Returns 'true' if a new parameter configuration has been recieved*/
    bool new_parameters();

    /* Returns 'true' if a new manual instruction has been recieved*/
    bool new_manual_instruction();

    /* Returns 'true' if a new manual instruction has been recieved*/
    bool new_semi_instruction();

    /* Returns 'true' if a new manual instruction has been recieved*/
    bool new_auto_instruction();

    /* Returns latest ParameterConfiguration recieved and resets new_parameters */
    ParameterConfiguration get_parameter_configuration();

    /* Returns latest ManualDriveInstruction recieved and resets new_manual_instruction */
    ManualDriveInstruction get_manual_drive_instruction();

    /* Returns latest SemiDriveInstruction recieved and resets new_semi_instruction */
    SemiDriveInstruction get_semi_drive_instruction();

    /* Returns latest AutoDriveInstruction recieved and resets new_auto_instruction */
    AutoDriveInstruction get_auto_drive_instruction();

private:
    void read();

    /* Variables for socket connection */
    int port;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;


    /* Variables to check if a new response has come */
    std::atomic<bool> parameters;
    std::atomic<bool> manual_instruction;
    std::atomic<bool> semi_instruction;
    std::atomic<bool> auto_instruction;

    std::atomic<bool> emergency_stop;
    std::atomic<bool> lost_connection;

    /* Variables to return upon request */
    ParameterConfiguration parameter_configuration;
    ManualDriveInstruction manual_drive_instruction;
    SemiDriveInstruction semi_drive_instruction;
    AutoDriveInstruction auto_drive_instruction;

    /* Threading */
    std::thread *thread;
    std::mutex mtx;
};

#endif  // CONNECTION_H
