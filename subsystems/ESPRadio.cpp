#include "subsystems/ESPRaido.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <fcntl.h>      // For fcntl()
#include <sys/ioctl.h>  // For FIONREAD
#include <unistd.h>     // For close(), read()
#include <errno.h>      // For errno
#include <string.h>     // For strerror()

// Function to print the contents of the received packet
void printPacket(const CommandPacket& packet) {
    std::cout << "--- Received Packet ---" << std::endl;
    std::cout << "Device ID: 0x" << std::hex << (int)packet.deviceID << std::endl;
    std::cout << "Command: 0x" << std::hex << (int)packet.command << std::endl;
    std::cout << "Type: 0x" << std::hex << (int)packet.type << std::endl;
    std::cout << "Value 1: " << std::dec << packet.value1 << std::endl;
    std::cout << "Value 2: " << std::dec << packet.value2 << std::endl;
    std::cout << "-----------------------" << std::endl;
}

// Class method implementations
ESPRadio::ESPRadio(boost::asio::io_context& io_context, const std::string& serial_port, unsigned int baud_rate)
    : io_context_(io_context), serial_port_(io_context, serial_port), buffer_() {
    
    // Configure the serial port
    serial_port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial_port_.set_option(boost::asio::serial_port_base::character_size(8));
    serial_port_.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial_port_.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial_port_.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    // CRITICAL: Set the serial port to non-blocking mode.
    int fd = serial_port_.native_handle();
    if (fd != -1) {
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Failed to set serial port to non-blocking mode." << std::endl;
        }
    } else {
        std::cerr << "Failed to get native handle for serial port." << std::endl;
    }
}

bool ESPRadio::readPacket(CommandPacket& packet) {
    // We now use the proven C-style read() call.
    try {
        int fd = serial_port_.native_handle();
        std::vector<uint8_t> temp_read_buffer(256);
        
        ssize_t bytes_read = read(fd, temp_read_buffer.data(), temp_read_buffer.size());
        
        if (bytes_read > 0) {
            buffer_.insert(buffer_.end(), temp_read_buffer.begin(), temp_read_buffer.begin() + bytes_read);
        } else if (bytes_read == -1) {
            // Check if the error is due to no data available.
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Error reading from serial port: " << strerror(errno) << std::endl;
                return false;
            }
        }
        
        // Now, check if our internal buffer contains a full packet.
        if (buffer_.size() >= sizeof(CommandPacket)) {
            std::memcpy(&packet, buffer_.data(), sizeof(CommandPacket));
            buffer_.erase(buffer_.begin(), buffer_.begin() + sizeof(CommandPacket));
            return true;
        }

        // Not enough data for a full packet yet, or no data was read.
        return false;

    } catch (const std::exception& e) {
        std::cerr << "Error in readPacket: " << e.what() << std::endl;
        return false;
    }
}