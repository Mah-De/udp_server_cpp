// g++ -std=c++11 -o test4 test4_udp_to_wav.cpp -lboost_system -pthread;


#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <csignal>
#include <array>
#include <unistd.h>

using boost::asio::ip::udp;

bool _stop_flag = false;

// WAV header parameters
const int sampleRate = 16000; // 16 kHz sample rate
const int numChannels = 2;    // Stereo (2 channels)
const int bitsPerSample = 32; // 32-bit samples
const int byteRate = sampleRate * numChannels * (bitsPerSample / 8);
const int blockAlign = numChannels * (bitsPerSample / 8);
int dataSize = 0; // Total size of audio data

void handleSigint(int sig)
{
    std::cout << "Caught SIGINT (Ctrl+C), exiting gracefully..." << std::endl;
    _stop_flag = true;
}

void writeWavHeader(std::ofstream &fileStream, int dataSize)
{
    // Create a WAV header for the file
    char header[44] = {};
    // RIFF Header
    memcpy(header, "RIFF", 4);
    *reinterpret_cast<int *>(&header[4]) = 36 + dataSize; // File size - 8
    memcpy(&header[8], "WAVE", 4);

    // fmt Chunk
    memcpy(&header[12], "fmt ", 4);
    *reinterpret_cast<int *>(&header[16]) = 16;  // Subchunk size
    *reinterpret_cast<short *>(&header[20]) = 1; // Audio format (1 = PCM)
    *reinterpret_cast<short *>(&header[22]) = numChannels;
    *reinterpret_cast<int *>(&header[24]) = sampleRate;
    *reinterpret_cast<int *>(&header[28]) = byteRate;
    *reinterpret_cast<short *>(&header[32]) = blockAlign;
    *reinterpret_cast<short *>(&header[34]) = bitsPerSample;

    // data Chunk
    memcpy(&header[36], "data", 4);
    *reinterpret_cast<int *>(&header[40]) = dataSize;

    fileStream.write(header, 44); // Write the header to the file
}

int num_msgs = 0;

int main()
{
    signal(SIGINT, handleSigint); // Register signal handler

    try
    {
        boost::asio::io_service io_service;
        udp::socket socket(io_service, udp::endpoint(udp::v4(), 1111));

        socket.non_blocking(true);

        std::ofstream file("/home/mahdy/Projects/udp_to_wav/output.wav", std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for writing!" << std::endl;
            return 1;
        }

        std::array<char, 1024> recvBuffer;
        udp::endpoint remoteEndpoint;

        bool headerWritten = false;
        std::cout << "Recording started ....." << std::endl;
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
                // No data available right now; continue
                continue;
            }
            else if (error && error != boost::asio::error::message_size)
            {
                std::cerr << "Error receiving data: " << error.message() << std::endl;
                break;
            }

            // Write WAV header once
            if (!headerWritten)
            {
                writeWavHeader(file, dataSize);
                headerWritten = true;
            }

            // Append the received data to the file
            file.write(recvBuffer.data(), bytes_transferred);
            num_msgs++;
            dataSize += bytes_transferred;

            // Print debug info
            // std::cout << "Received " << bytes_transferred << " bytes. Total data size: " << dataSize << " bytes" << std::endl;
        }

        // After receiving is complete, close the file
        file.close();
        std::cout << "Finished writing the WAV file! " << num_msgs << " messages written." << std::endl;
        
    }
    catch (const std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}







// #include <iostream>
// #include <fstream>
// #include <boost/asio.hpp>
// #include <csignal>
// #include <array>
// #include <unistd.h>

// using boost::asio::ip::udp;

// bool _stop_flag = false;

// // WAV header parameters
// const int sampleRate = 16000;  // 16 kHz sample rate
// const int numChannels = 2;     // Stereo (2 channels)
// const int bitsPerSample = 16;  // 16-bit samples
// const int byteRate = sampleRate * numChannels * (bitsPerSample / 8);
// const int blockAlign = numChannels * (bitsPerSample / 8);
// int dataSize = 0;  // Total size of audio data

// void handleSigint(int sig) {
//     std::cout << "Caught SIGINT (Ctrl+C), exiting gracefully..." << std::endl;
//     _stop_flag = true;
// }

// void writeWavHeader(std::ofstream &fileStream, int dataSize) {
//     // Create a WAV header for the file
//     char header[44] = {};
//     // RIFF Header
//     memcpy(header, "RIFF", 4);
//     *reinterpret_cast<int*>(&header[4]) = 36 + dataSize;  // File size - 8
//     memcpy(&header[8], "WAVE", 4);
    
//     // fmt Chunk
//     memcpy(&header[12], "fmt ", 4);
//     *reinterpret_cast<int*>(&header[16]) = 16;  // Subchunk size
//     *reinterpret_cast<short*>(&header[20]) = 1;  // Audio format (1 = PCM)
//     *reinterpret_cast<short*>(&header[22]) = numChannels;
//     *reinterpret_cast<int*>(&header[24]) = sampleRate;
//     *reinterpret_cast<int*>(&header[28]) = byteRate;
//     *reinterpret_cast<short*>(&header[32]) = blockAlign;
//     *reinterpret_cast<short*>(&header[34]) = bitsPerSample;
    
//     // data Chunk
//     memcpy(&header[36], "data", 4);
//     *reinterpret_cast<int*>(&header[40]) = dataSize;

//     fileStream.write(header, 44);  // Write the header to the file
// }

// int main() {
//     signal(SIGINT, handleSigint);  // Register signal handler

//     try {
//         boost::asio::io_service io_service;
//         udp::socket socket(io_service, udp::endpoint(udp::v4(), 1111));

//         socket.non_blocking(true);

//         std::ofstream file("/home/mahdy/Projects/udp_to_wav/output.wav", std::ios::out | std::ios::binary | std::ios::trunc);
//         if (!file.is_open()) {
//             std::cerr << "Failed to open file for writing!" << std::endl;
//             return 1;
//         }

//         std::array<char, 1024> recvBuffer;
//         udp::endpoint remoteEndpoint;

//         bool headerWritten = false;

//         while (true) {
//             if (_stop_flag) {
//                 std::cout << "Caught SIGINT (Ctrl+C), BREAKING" << std::endl;
//                 break;
//             }

//             boost::system::error_code error;
//             std::size_t bytes_transferred = socket.receive_from(boost::asio::buffer(recvBuffer), remoteEndpoint, 0, error);

//             if (error == boost::asio::error::would_block) {
//                 // No data available right now, let's wait and retry
//                 usleep(1000);  // Sleep for 1ms
//                 continue;
//             } else if (error && error != boost::asio::error::message_size) {
//                 std::cerr << "Error receiving data: " << error.message() << std::endl;
//                 break;
//             }

//             std::cout << "Received " << bytes_transferred << " bytes from " << remoteEndpoint.address().to_string() << std::endl;

//             // Write WAV header once
//             if (!headerWritten) {
//                 writeWavHeader(file, dataSize);
//                 headerWritten = true;
//             }

//             // Append the received data to the file
//             file.write(recvBuffer.data(), bytes_transferred);
//             dataSize += bytes_transferred;

//             // Debug: print data size so far
//             std::cout << "Data size so far: " << dataSize << " bytes" << std::endl;
//         }

//         // After receiving is complete, close the file
//         file.close();
//         std::cout << "Finished writing the WAV file!" << std::endl;
//     } catch (const std::exception &ex) {
//         std::cerr << ex.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
