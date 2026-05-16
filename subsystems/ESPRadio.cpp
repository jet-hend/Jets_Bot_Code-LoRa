#include "subsystems/ESPRaido.h"

#include <algorithm>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

void printLoRaFrame(const LoRaFrame& frame) {
    std::cout << "--- LoRa Frame ---\n"
              << "  pkt=0x" << std::hex << static_cast<int>(loraHeaderPktType(frame.header))
              << " dev=0x" << static_cast<int>(loraHeaderDeviceId(frame.header))
              << " cmd=0x" << static_cast<int>(frame.command) << " seq=" << std::dec
              << static_cast<int>(loraSeq(frame)) << " ack=" << loraWantsAck(frame)
              << " urgent=" << loraIsUrgent(frame) << "\n"
              << "  v1=" << frame.value1 << " v2=" << frame.value2 << "\n"
              << "------------------\n";
}

void printPacket(const CommandPacket& packet) {
    std::cout << "--- Legacy Packet ---\n"
              << "  dev=0x" << std::hex << static_cast<int>(packet.deviceID) << " cmd=0x"
              << static_cast<int>(packet.command) << " type=0x" << static_cast<int>(packet.type) << std::dec
              << "\n  v1=" << packet.value1 << " v2=" << packet.value2 << "\n"
              << "---------------------\n";
}

ESPRadio::ESPRadio(boost::asio::io_context& io_context, const std::string& serial_port, unsigned int baud_rate)
    : io_context_(io_context), serial_port_(io_context, serial_port), buffer_() {
    serial_port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial_port_.set_option(boost::asio::serial_port_base::character_size(8));
    serial_port_.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial_port_.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial_port_.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    const int fd = serial_port_.native_handle();
    if (fd != -1) {
        const int flags = fcntl(fd, F_GETFL);
        if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Failed to set serial port to non-blocking mode.\n";
        }
    } else {
        std::cerr << "Failed to get native handle for serial port.\n";
    }
}

bool ESPRadio::readFrame(LoRaFrame& frame) {
    try {
        const int fd = serial_port_.native_handle();
        std::vector<uint8_t> temp(256);

        const ssize_t bytes_read = read(fd, temp.data(), temp.size());
        if (bytes_read > 0) {
            buffer_.insert(buffer_.end(), temp.begin(), temp.begin() + bytes_read);
        } else if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Serial read error: " << strerror(errno) << "\n";
            return false;
        }

        while (buffer_.size() >= LORA_FRAME_SIZE) {
            const auto magicIt = std::find(buffer_.begin(), buffer_.end(), LORA_MAGIC);
            if (magicIt == buffer_.end()) {
                buffer_.clear();
                return false;
            }
            if (magicIt != buffer_.begin()) {
                buffer_.erase(buffer_.begin(), magicIt);
            }
            if (buffer_.size() < LORA_FRAME_SIZE) {
                return false;
            }

            std::memcpy(&frame, buffer_.data(), LORA_FRAME_SIZE);
            buffer_.erase(buffer_.begin(), buffer_.begin() + LORA_FRAME_SIZE);
            return true;
        }

        return false;
    } catch (const std::exception& e) {
        std::cerr << "readFrame: " << e.what() << "\n";
        return false;
    }
}

bool ESPRadio::readPacket(CommandPacket& packet) {
    LoRaFrame frame{};
    if (!readFrame(frame)) {
        return false;
    }
    packet.deviceID = loraHeaderDeviceId(frame.header);
    packet.command = loraHeaderPktType(frame.header);
    packet.type = frame.command;
    packet.value1 = frame.value1;
    packet.value2 = frame.value2;
    return true;
}
