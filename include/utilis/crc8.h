#pragma once

#include <cstdint>
#include <cstddef>
#include "cmd_protocol.h" // Your packet file

class CRC8Processor {
private:
    static constexpr uint8_t POLYNOMIAL = 0x31;

    // A compile-time helper class to build the 256-byte array
    struct LUTGenerator {
        uint8_t table[256]{};
        
        constexpr LUTGenerator() {
            for (int i = 0; i < 256; ++i) {
                uint8_t crc = static_cast<uint8_t>(i);
                for (int j = 0; j < 8; ++j) {
                    if (crc & 0x80) {
                        crc = (crc << 1) ^ POLYNOMIAL;
                    } else {
                        crc <<= 1;
                    }
                }
                table[i] = crc;
            }
        }
    };

    // This creates the table at compile time and stores it in Flash memory
    static constexpr LUTGenerator lut = LUTGenerator();

public:
    // Blazing-fast byte-by-byte lookup function
    static uint8_t calculate(const uint8_t* data, size_t length) {
        uint8_t crc = 0x00; // Initial value
        for (size_t i = 0; i < length; ++i) {
            // XOR incoming byte with current CRC, then grab the precomputed remainder
            crc = lut.table[crc ^ data[i]];
        }
        return crc;
    }

    // Helper explicitly designed for your 9-byte packet structure
    static bool verify_packet(const PacketStructure& pkt) {
        // Calculate CRC on the first 8 bytes of the packet
        uint8_t calculated_crc = calculate(reinterpret_cast<const uint8_t*>(&pkt), 8);
        return calculated_crc == pkt.crc8;
    }
};