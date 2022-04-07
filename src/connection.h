#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <boost/asio.hpp>
#include <json.hpp>

#include "manualdriveinstruction.h"
#include "semidriveinstruction.h"
#include "autodriveinstruction.h"

using json = nlohmann::json;

/* Custom exception for connection loss */
struct LostConnectionError : public std::exception {
    const char * what () const throw () {
        return "Lost connection with user interface";
    }
};

class Connection {
public:
    Connection(int port);

    /* Basic function */
    void restart();
    void read();
    void write(const std::string& response);

    /* New-functions */
    bool new_manual_instruction();
    bool new_semi_instruction();
    bool new_auto_instruction();

    /* Getters */
    ManualDriveInstruction get_manual_drive_instruction();
    SemiDriveInstruction get_semi_drive_instruction();
    AutoDriveInstruction get_auto_drive_instruction();

private:
    /* Variables for socket connection */
    int port;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;

    /* Variables to check if a new instructions has come */
    bool manual_instruction;
    bool semi_instruction;
    bool auto_instruction;

    /* Variables to return upon request */
    ManualDriveInstruction manual_drive_instruction;
    SemiDriveInstruction semi_drive_instruction;
    AutoDriveInstruction auto_drive_instruction;

};

#endif  // CONNECTION_H
