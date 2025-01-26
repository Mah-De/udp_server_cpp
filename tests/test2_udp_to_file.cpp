#include <string>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::udp;

namespace {

class HelloWorldServer {
public:
    HelloWorldServer(boost::asio::io_service& io_service, const std::string& filename)
        : _socket(io_service, udp::endpoint(udp::v4(), 1111)), _filename(filename)
    {
        _file.open(_filename, std::ios::out | std::ios::app);  // Open the file in append mode
        if (!_file.is_open()) {
            std::cerr << "Failed to open file for writing!" << std::endl;
            return;
        }
        startReceive();
    }

    ~HelloWorldServer() {
        if (_file.is_open()) {
            _file.close();  // Close the file when done
        }
    }

private:
    void startReceive() {
        _socket.async_receive_from(
            boost::asio::buffer(_recvBuffer), _remoteEndpoint,
            boost::bind(&HelloWorldServer::handleReceive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handleReceive(const boost::system::error_code& error,
                       std::size_t bytes_transferred) {
        if (!error || error == boost::asio::error::message_size) {
            // Convert the received buffer to a string
            std::string message(_recvBuffer.begin(), _recvBuffer.begin() + bytes_transferred);

            // Print the received message to the terminal
            std::cout << "Received message: " << message << std::endl;

            // Write the received message to the file
            _file << message << std::endl;
            if (_file.fail()) {
                std::cerr << "Error writing to file!" << std::endl;
            }

            // Continue receiving data
            startReceive();
        }
    }

    udp::socket _socket;
    udp::endpoint _remoteEndpoint;
    std::array<char, 1024> _recvBuffer;
    std::ofstream _file;
    std::string _filename;
};

}  // namespace

int main() {
    try {
        boost::asio::io_service io_service;
        HelloWorldServer server{io_service, "/home/mahdy/Projects/udp_to_wav/test2_output.txt"};
        io_service.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}

