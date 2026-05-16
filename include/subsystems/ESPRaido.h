#ifndef ESPRADIO_H
#define ESPRADIO_H

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "cmd_protocol.h"

void printLoRaFrame(const LoRaFrame& frame);
void printPacket(const CommandPacket& packet);

class ESPRadio {
public:
    ESPRadio(boost::asio::io_context& io_context, const std::string& serial_port, unsigned int baud_rate);

    bool readFrame(LoRaFrame& frame);

    /// @deprecated Parses v0.2 frame into legacy CommandPacket layout
    bool readPacket(CommandPacket& packet);

private:
    boost::asio::io_context& io_context_;
    boost::asio::serial_port serial_port_;
    std::vector<uint8_t> buffer_;
};

#endif
