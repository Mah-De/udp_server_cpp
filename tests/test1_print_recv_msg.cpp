#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::udp;

namespace {

class HelloWorldServer {
public:
    HelloWorldServer(boost::asio::io_service& io_service)
        : _socket(io_service, udp::endpoint(udp::v4(), 1111))
    {
        startReceive();
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
            // Print the received message
            std::string message(_recvBuffer.begin(), _recvBuffer.begin() + bytes_transferred);
            std::cout << "Received message: " << message << std::endl;

            // Optionally send a response (comment out if not needed)
            auto response = std::make_shared<std::string>("Hello, World\n");
            _socket.async_send_to(boost::asio::buffer(*response), _remoteEndpoint,
                boost::bind(&HelloWorldServer::handleSend, this, response,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

    void handleSend(std::shared_ptr<std::string> message,
                    const boost::system::error_code& ec,
                    std::size_t bytes_transferred) {
        startReceive();  // Continue receiving the next message
    }

    udp::socket _socket;
    udp::endpoint _remoteEndpoint;
    std::array<char, 1024> _recvBuffer;
};

}  // namespace

int main() {
    try {
        boost::asio::io_service io_service;
        HelloWorldServer server{io_service};
        io_service.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}
