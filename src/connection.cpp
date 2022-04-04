#include <string>
#include <boost/asio.hpp>

#include "connection.h"

using namespace boost::asio;
using ip::tcp;

/* Establish a connection on specified port, not done until a client respondes */
Connection::Connection(int port)
: port{port}, io_service{},
  acceptor{io_service, tcp::endpoint(tcp::v4(), port)}, socket{io_service} {
      acceptor.accept(socket);
}

/* Reestablishe connection, same operations as in constructor */
void Connection::restart() {
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port)); // listen for new connection
      tcp::socket socket(io_service);  // socket creation
      acceptor.accept(socket);  // waiting for connection
}

/* Recieve a string from the client */
std::string Connection::read() {
      boost::asio::streambuf buf;
      boost::asio::read_until( socket, buf, "\n" );
      std::string request = boost::asio::buffer_cast<const char*>(buf.data());
      return request;
}

/* Send a string to the client */
void Connection::write(const std::string& response) {
      const std::string msg = response + "\n";
      boost::asio::write( socket, boost::asio::buffer(response));
}
