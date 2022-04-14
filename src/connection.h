#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "manualdriveinstruction.h"
#include "semidriveinstruction.h"
#include "autodriveinstruction.h"

using json = nlohmann::json;

class Connection {
public:
    Connection(int port);
    ~Connection();

    /* Delete unused methods */
    Connection(const Connection&) = delete;
    Connection operator=(const Connection&) = delete;

    /* Basic function */
    void restart();
    void write(const std::string& response);
    bool has_lost_connection();

    /* Emergency stop */
    bool emergency_recieved();

    /* New-functions */
    bool new_manual_instruction();
    bool new_semi_instruction();
    bool new_auto_instruction();

    /* Getters */
    ManualDriveInstruction get_manual_drive_instruction();
    SemiDriveInstruction get_semi_drive_instruction();
    AutoDriveInstruction get_auto_drive_instruction();

private:
    void read();

    /* Variables for socket connection */
    int port;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;

    /* Variables to check if a new instructions has come */
    std::atomic<bool> emergency_stop;
    std::atomic<bool> manual_instruction;
    std::atomic<bool> semi_instruction;
    std::atomic<bool> auto_instruction;

    std::atomic<bool> lost_connection;

    /* Variables to return upon request */
    ManualDriveInstruction manual_drive_instruction;
    SemiDriveInstruction semi_drive_instruction;
    AutoDriveInstruction auto_drive_instruction;

    /* Threading */
    std::thread *thread;
    std::mutex mtx;
};

#endif  // CONNECTION_H
