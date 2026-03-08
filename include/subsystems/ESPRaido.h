#ifndef ESPRADIO_H
#define ESPRADIO_H

#include <cstdint> // for uint8_t and int16_t
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <vector>
#include <cstring>
#include "cmd_protocol.h"

// Function to print the contents of the received packet.
void printPacket(const CommandPacket& packet);

// Class to manage the serial communication.
class ESPRadio {
public:
    // Constructor to initialize the serial port.
    ESPRadio(boost::asio::io_context& io_context, const std::string& serial_port, unsigned int baud_rate);

    // Method to read a single CommandPacket from the serial port.
    bool readPacket(CommandPacket& packet);

private:
    boost::asio::io_context& io_context_;
    boost::asio::serial_port serial_port_;
    std::vector<uint8_t> buffer_;
};

#endif // ESPRADIO_H