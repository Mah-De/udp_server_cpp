#include <string>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <iostream>
#include <csignal>
#include <unistd.h>

using boost::asio::ip::udp;

bool _stop_flag = false;

void handleSigint(int sig)
{
    std::cout << "Caught SIGINT (Ctrl+C), exiting gracefully..." << std::endl;
    // Perform any cleanup here if needed
    _stop_flag = true;
}

int main()
{
    // Register signal handler
    signal(SIGINT, handleSigint);

    try
    {
        boost::asio::io_service io_service;

        // Set up the UDP socket and endpoint
        udp::socket socket(io_service, udp::endpoint(udp::v4(), 1111));

        // Set socket to non-blocking mode
        socket.non_blocking(true);

        // Open the output file in append mode
        std::ofstream file("/home/mahdy/Projects/udp_to_wav/output.txt", std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for writing!" << std::endl;
            return 1;
        }

        std::array<char, 1024> recvBuffer;
        udp::endpoint remoteEndpoint;

        // Start the receive loop
        std::cout << "Press Enter to stop receiving messages..." << std::endl;
        while (true)
        {

            if (_stop_flag)
            {
                std::cout << "Caught SIGINT (Ctrl+C), BREAKING" << std::endl;
                break;
            }

            boost::system::error_code error;
            std::size_t bytes_transferred = socket.receive_from(boost::asio::buffer(recvBuffer), remoteEndpoint, 0, error);

            if (error == boost::asio::error::would_block)
            {
                // No data available right now; you can do other work here
                // For example, you could sleep for a while or handle other tasks
                continue;
            }
            else if (error && error != boost::asio::error::message_size)
            {
                // If any other error occurs, handle it
                std::cerr << "Error receiving data: " << error.message() << std::endl;
                break;
            }

            // Convert the received buffer to a string
            std::string message(recvBuffer.begin(), recvBuffer.begin() + bytes_transferred);

            // Print the received message to the terminal
            std::cout << "Received message: " << message << std::endl;

            // Write the received message to the file
            file << message; // << std::endl;
            if (file.fail())
            {
                std::cerr << "Error writing to file!" << std::endl;
                break;
            }
        }

        // Close the file
        file.close();
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
















// // #include <string>
// // #include <iostream>
// // #include <fstream>
// // #include <boost/asio.hpp>

// // using boost::asio::ip::udp;

// // namespace {

// // class HelloWorldServer {
// // public:
// //     HelloWorldServer(boost::asio::io_service& io_service, const std::string& filename)
// //         : _socket(io_service, udp::endpoint(udp::v4(), 1111)), _filename(filename)
// //     {
// //         _file.open(_filename, std::ios::out | std::ios::app);  // Open the file in append mode
// //         if (!_file.is_open()) {
// //             std::cerr << "Failed to open file for writing!" << std::endl;
// //             return;
// //         }
// //     }

// //     ~HelloWorldServer() {
// //         if (_file.is_open()) {
// //             _file.close();  // Close the file when done
// //         }
// //     }

// //     void startReceive() {
// //         while (true) {
// //             boost::system::error_code error;
// //             std::size_t bytes_transferred = _socket.receive_from(boost::asio::buffer(_recvBuffer), _remoteEndpoint, 0, error);

// //             if (error && error != boost::asio::error::message_size) {
// //                 std::cerr << "Error receiving data: " << error.message() << std::endl;
// //                 break;
// //             }

// //             // Convert the received buffer to a string
// //             std::string message(_recvBuffer.begin(), _recvBuffer.begin() + bytes_transferred);

// //             // Print the received message to the terminal
// //             std::cout << "Received message: " << message << std::endl;

// //             // Write the received message to the file
// //             _file << message << std::endl;
// //             if (_file.fail()) {
// //                 std::cerr << "Error writing to file!" << std::endl;
// //                 break;
// //             }
// //         }
// //     }

// // private:
// //     udp::socket _socket;
// //     udp::endpoint _remoteEndpoint;
// //     std::array<char, 1024> _recvBuffer;
// //     std::ofstream _file;
// //     std::string _filename;
// // };

// // }  // namespace

// // int main() {
// //     try {
// //         std::cout << "[TRY] started" << std::endl;
// //         boost::asio::io_service io_service;
// //         HelloWorldServer server{io_service, "/home/mahdy/Projects/udp_to_wav/test2_output.txt"};
// //         server.startReceive();
// //         std::cout << "[TRY] ENDED" << std::endl;
// //     } catch (const std::exception& ex) {
// //         std::cerr << ex.what() << std::endl;
// //     }
// //     return 0;
// // }

// #include <string>
// #include <iostream>
// #include <fstream>
// #include <boost/asio.hpp>
// #include <thread>
// #include <atomic>

// using boost::asio::ip::udp;

// namespace {

// class HelloWorldServer {
// public:
//     HelloWorldServer(boost::asio::io_service& io_service, const std::string& filename)
//         : _socket(io_service, udp::endpoint(udp::v4(), 1111)), _filename(filename), _stop(false)
//     {
//         _file.open(_filename, std::ios::out | std::ios::app);  // Open the file in append mode
//         if (!_file.is_open()) {
//             std::cerr << "Failed to open file for writing!" << std::endl;
//             return;
//         }
//     }

//     ~HelloWorldServer() {
//         if (_file.is_open()) {
//             _file.close();  // Close the file when done
//         }
//     }

//     void startReceive() {
//         // Start a separate thread to monitor Enter key press
//         std::thread input_thread(&HelloWorldServer::monitorInput, this);

//         while (true) {
//             // If _stop is set to true (Enter was pressed), break the loop
//             if (_stop.load()) {
//                 std::cout << "Stopping the server..." << std::endl;
//                 break;
//             }

//             boost::system::error_code error;
//             std::size_t bytes_transferred = _socket.receive_from(boost::asio::buffer(_recvBuffer), _remoteEndpoint, 0, error);

//             if (error && error != boost::asio::error::message_size) {
//                 std::cerr << "Error receiving data: " << error.message() << std::endl;
//                 break;
//             }

//             // Convert the received buffer to a string
//             std::string message(_recvBuffer.begin(), _recvBuffer.begin() + bytes_transferred);

//             // Print the received message to the terminal
//             std::cout << "Received message: " << message << std::endl;

//             // Write the received message to the file
//             _file << message << std::endl;
//             if (_file.fail()) {
//                 std::cerr << "Error writing to file!" << std::endl;
//                 break;
//             }
//         }

//         // Ensure the input thread is joined before finishing
//         input_thread.join();
//     }

// private:
//     void monitorInput() {
//         std::cout << "Press Enter to stop receiving messages..." << std::endl;
//         std::cin.get();  // Wait for Enter key press
//         _stop.store(true);  // Set the flag to stop the while loop
//     }

//     udp::socket _socket;
//     udp::endpoint _remoteEndpoint;
//     std::array<char, 1024> _recvBuffer;
//     std::ofstream _file;
//     std::string _filename;
//     std::atomic<bool> _stop;  // Flag to stop the loop when Enter is pressed
// };

// }  // namespace

// int main() {
//     try {
//         std::cout << "[TRY] started" << std::endl;
//         boost::asio::io_service io_service;
//         HelloWorldServer server{io_service, "/home/mahdy/Projects/udp_to_wav/test3_output.txt"};
//         server.startReceive();
//         std::cout << "[TRY] ENDED" << std::endl;
//     } catch (const std::exception& ex) {
//         std::cerr << ex.what() << std::endl;
//     }
//     return 0;
// }