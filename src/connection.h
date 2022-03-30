#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <boost/asio.hpp>

class Connection {
public:
    Connection(int port);

    void restart();
    std::string read();
    void write(const std::string& response);

private:
    int port;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
};

#endif  // CONNECTION_H
