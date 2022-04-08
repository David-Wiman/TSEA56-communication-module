#include "connection.h"

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

/* Check if a given key exists in an json object */
bool exists(const json& j, const std::string& key) {
    return j.find(key) != j.end();
}

/* Establish a connection on specified port, not done until a client respondes */
Connection::Connection(int port)
: port{port}, io_service{}, acceptor{io_service, tcp::endpoint(tcp::v4(), port)},
  socket{io_service}, manual_instruction{false}, semi_instruction{false},
  auto_instruction{false}, lost_connection{false}, manual_drive_instruction{},
  semi_drive_instruction{}, auto_drive_instruction{}, thread{}, mtx{} {
    acceptor.accept(socket);
    thread = new std::thread(&Connection::read, this);
    cout << "Succesfully connected to port " << port << endl;
}

Connection::~Connection() {
    thread->join();
    delete thread;
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
    while (true) {
        try {
            // Continuously read until newline, create json object from string
            boost::asio::streambuf buf;
            cout << "Connection: Read until" << endl;
            boost::asio::read_until( socket, buf, "\n" );
            cout << "Connection: Data recieved" << endl;
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
                std::lock_guard<std::mutex> lk(mtx);
                manual_instruction.store(true);
                ManualDriveInstruction inst{j};
                manual_drive_instruction = inst;
            } else if (exists(j, "SemiDriveInstruction")) {
                std::lock_guard<std::mutex> lk(mtx);
                semi_instruction.store(true);
                SemiDriveInstruction inst{j};
                semi_drive_instruction = inst;
            } else if (exists(j, "AutoDriveInstruction")) {
                std::lock_guard<std::mutex> lk(mtx);
                auto_instruction.store(true);
                AutoDriveInstruction inst{j};
                auto_drive_instruction = inst;
            }
        } catch (const boost::exception&) {
            lost_connection.store(true);
            break;
        }
    }
}

/* Send a string to the client */
void Connection::write(const std::string& response) {
    const std::string msg = response + "\n";
    boost::asio::write( socket, boost::asio::buffer(response));
}

bool Connection::has_lost_connection() {
    return lost_connection.load();
}

/* Functions to check if a new value exists*/
bool Connection::new_manual_instruction() {
    return manual_instruction.load();
}

bool Connection::new_semi_instruction() {
    return semi_instruction.load();
}

bool Connection::new_auto_instruction() {
    return auto_instruction.load();
}

/* Gettersn, sets new-values to false*/
ManualDriveInstruction Connection::get_manual_drive_instruction() {
    std::lock_guard<std::mutex> lk(mtx);
    manual_instruction.store(false);
    return manual_drive_instruction;
}

SemiDriveInstruction Connection::get_semi_drive_instruction() {
    std::lock_guard<std::mutex> lk(mtx);
    semi_instruction.store(false);
    return semi_drive_instruction;
}

AutoDriveInstruction Connection::get_auto_drive_instruction() {
    std::lock_guard<std::mutex> lk(mtx);
    auto_instruction.store(false);
    return auto_drive_instruction;
}
