#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

string read_(tcp::socket & socket) {
       boost::asio::streambuf buf;
       boost::asio::read_until( socket, buf, "\n" );
       string data = boost::asio::buffer_cast<const char*>(buf.data());
       return data;
}
void send_(tcp::socket & socket, const string& message) {
       const string msg = message + "\n";
       boost::asio::write( socket, boost::asio::buffer(message) );
}

int main() {
      cout << "start" << endl;
      string message = "init";
      boost::asio::io_service io_service;
//listen for new connection
      cout << "listen for connection" << endl;
      tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234 ));
//socket creation
      cout << "socket creation" << endl;
      tcp::socket socket_(io_service);
//waiting for connection
      cout << "wainting for connection" << endl;
      acceptor_.accept(socket_);
//read operation
      cout << "read" << endl;
      message = read_(socket_);
      cout << message << endl;
//write operation
      cout << "write" << endl;
      send_(socket_, "Hello From Server!");
      cout << "Servent sent Hello message to Client!" << endl;
   return 0;
}
