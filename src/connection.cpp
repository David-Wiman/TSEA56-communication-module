/* Written by David Wiman, 6/4 -22 */

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

/* Reestablishe connection, same operations as in constructor */
void Connection::restart() {
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port)); // listen for new connection
      tcp::socket socket(io_service);  // socket creation
      acceptor.accept(socket);  // waiting for connection
}

/* Recieve a string from the client, set new_instruction, create instruction object */
void Connection::read() {
      try {
            boost::asio::streambuf buf;
            boost::asio::read_until( socket, buf, "\n" );
            std::string request = boost::asio::buffer_cast<const char*>(buf.data());
            json j = json::parse(request);
            
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

/* New */
bool Connection::new_manual_instruction() {
      return manual_instruction;
}

bool Connection::new_semi_instruction() {
      return semi_instruction;
}

bool Connection::new_auto_instruction() {
      return auto_instruction;
}

/* Getters */
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
