#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

//ändra "Server" till "Connection"

class Connection
{
      public:
            Connection(int port);
            
            void restart();
            string read();
            void write(const string& response);
      
      private:
            int port;
            boost::asio::io_service io_service;
            tcp::acceptor acceptor;
            tcp::socket socket;
};

int main()
{     
      cout << "börjar" << endl;
      Connection connection{1234};
      cout << "klar" << endl;
      cout << "redo att läsa" << endl;
      connection.read();     
      cout << "färdigläst" << endl;
      return 0;
}

Connection::Connection(int port)
      :port{port}, io_service{}, acceptor{io_service, tcp::endpoint(tcp::v4(), port)}, socket{io_service}
      {
            acceptor.accept(socket);
      }
      
void Connection::restart()
{
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port)); //listen for new connection
      tcp::socket socket(io_service);                                   //socket creation
      acceptor.accept(socket);                                          //waiting for connection
}

string Connection::read()
{
      boost::asio::streambuf buf;
      boost::asio::read_until( socket, buf, "\n" );
      string request = boost::asio::buffer_cast<const char*>(buf.data());
      return request;
}

void Connection::write(const string& response)
{
      const string msg = response + "\n";
      boost::asio::write( socket, boost::asio::buffer(response));
}
