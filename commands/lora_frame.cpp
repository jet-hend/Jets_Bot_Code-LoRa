#include "cmd_protocol.h"

uint8_t loraCrc8(const uint8_t* data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x80) {
                crc = static_cast<uint8_t>((crc << 1) ^ 0x07);
            } else {
                crc = static_cast<uint8_t>(crc << 1);
            }
        }
    }
    return crc;
}

bool loraValidateFrame(const LoRaFrame& frame) {
    if (frame.magic != LORA_MAGIC) {
        return false;
    }
    const auto* bytes = reinterpret_cast<const uint8_t*>(&frame);
    return frame.crc == loraCrc8(bytes, LORA_FRAME_SIZE - 1);
}

void loraSetCrc(LoRaFrame& frame) {
    frame.magic = LORA_MAGIC;
    auto* bytes = reinterpret_cast<uint8_t*>(&frame);
    frame.crc = loraCrc8(bytes, LORA_FRAME_SIZE - 1);
}
