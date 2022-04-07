#include <iostream>

#include "connection.h"

using namespace boost::asio;
using ip::tcp;

/* Check if a given key exists in an json object */
bool exists(const json& j, const std::string& key) {
    return j.find(key) != j.end();
}

/* Establish a connection on specified port, not done until a client respondes */
Connection::Connection(int port)
: port{port}, io_service{}, acceptor{io_service, tcp::endpoint(tcp::v4(),
  port)},socket{io_service}, manual_instruction{false},
  semi_instruction{false}, auto_instruction{false},
  manual_drive_instruction{}, semi_drive_instruction{}, auto_drive_instruction{} {
    acceptor.accept(socket);
}

/* Reestablish connection, same operations as in constructor */
void Connection::restart() {
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port)); // listen for new connection
    tcp::socket socket(io_service);  // socket creation
    acceptor.accept(socket);  // waiting for connection
}

/* Recieve a string from the client, set new_instruction, create instruction object */
void Connection::read() {
    try {
        // Continuously read until newline, create json object from string
        boost::asio::streambuf buf;
        boost::asio::read_until( socket, buf, "\n" );
        std::string request = boost::asio::buffer_cast<const char*>(buf.data());

        json j{};
        try {
            j = json::parse(request);
        } catch (std::invalid_argument&) {
            std::cout << "invalid argument" << std::endl;
            return;
        }

        // Check what kind of instruction and create instance of appropriate class
        if (exists(j, "ManualDriveInstruction")) {
            manual_instruction = true;
            ManualDriveInstruction inst{j};
            manual_drive_instruction = inst;
        } else if (exists(j, "SemiDriveInstruction")) {
            semi_instruction = true;
            SemiDriveInstruction inst{j};
            semi_drive_instruction = inst;
        } else if (exists(j, "AutoDriveInstruction")) {
            auto_instruction = true;
            AutoDriveInstruction inst{j};
            auto_drive_instruction = inst;
        }
    } catch (const boost::exception&) {
        throw LostConnectionError();
    }
}

/* Send a string to the client */
void Connection::write(const std::string& response) {
    const std::string msg = response + "\n";
    boost::asio::write( socket, boost::asio::buffer(response));
}

/* Functions to check if a new value exists*/
bool Connection::new_manual_instruction() {
    return manual_instruction;
}

bool Connection::new_semi_instruction() {
    return semi_instruction;
}

bool Connection::new_auto_instruction() {
    return auto_instruction;
}

/* Gettersn, sets new-values to false*/
ManualDriveInstruction Connection::get_manual_drive_instruction() {
    manual_instruction = false;
    return manual_drive_instruction;
}

SemiDriveInstruction Connection::get_semi_drive_instruction() {
    semi_instruction = false;
    return semi_drive_instruction;
}

AutoDriveInstruction Connection::get_auto_drive_instruction() {
    auto_instruction = false;
    return auto_drive_instruction;
}
