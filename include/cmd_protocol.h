#pragma once

#include <cstddef>
#include <cstdint>

/// LoRa packet protocol v0.2 (see spreadsheet: LoRa CMD / LoRa Frame v0.2)

constexpr uint8_t LORA_MAGIC = 0xA5;
constexpr size_t LORA_FRAME_SIZE = 9;
constexpr uint8_t LORA_DEVICE_ID = 0x01;

#pragma pack(push, 1)
/// 9-byte on-wire frame: magic | header | seqFlags | command | payload(4) | crc
struct LoRaFrame {
    uint8_t magic;
    uint8_t header;
    uint8_t seqFlags;
    uint8_t command;
    int16_t value1;
    int16_t value2;
    uint8_t crc;
};
#pragma pack(pop)

static_assert(sizeof(LoRaFrame) == LORA_FRAME_SIZE);

/// Packet type (PKT_HEX column — high nibble of header for types < 0x80)
enum LoRaPktType : uint8_t {
    PKT_STATE = 0x01,
    PKT_COMMAND = 0x02,
    PKT_HEARTBEAT = 0x03,
    PKT_TELEMETRY = 0x04,
    PKT_ERROR = 0x05,
    PKT_DEBUG = 0x06,
    PKT_RESPONSE = 0x81,
};

enum StateCmd : uint8_t {
    STATE_ESTOP = 0x00,
    STATE_CSTOP = 0x01,
    STATE_DISABLED = 0x02,
    STATE_ENABLED = 0x03,
};

enum DriveCmd : uint8_t {
    DRIVE_STOP = 0x00,
    DRIVE_TANK = 0x01,
    DRIVE_ARC = 0x02,
    DRIVE_HEADING = 0x03,
    DRIVE_SET_BREAKMODE = 0x04,
    DRIVE_SET_SOLBREAKS = 0x05,
    DRIVE_MAX_VELOCITY = 0x06,
    DRIVE_TRIM = 0x07,
    STM_REBOOT = 0x08,
    STM_FAULTS_CLEAR = 0x09,
    STM_GYRO_CALI = 0x10,
    STM_REQ_ERROR = 0x11,
    STM_SET_DRIVE_PID = 0x12,
};

enum HeartbeatCmd : uint8_t {
    HEARTBEAT = 0x00,
    HEARTBEAT_EXT = 0x01,
    HEARTBEAT_STATUS = 0x02,
    HEARTBEAT_REQ = 0x03,
};

enum TelemetryCmd : uint8_t {
    TELEM_DRIVE = 0x00,
    TELEM_ODOMETRY = 0x01,
    TELEM_BATTERY = 0x02,
    TELEM_IMU = 0x03,
    TELEM_FULL = 0x04,
    TELEM_REQ = 0x05,
};

enum ErrorCmd : uint8_t {
    ERROR_FAULT = 0x00,
    ERROR_MOTOR = 0x01,
    ERROR_SENSOR = 0x02,
    ERROR_COMMS = 0x03,
    ERROR_CLEAR = 0x04,
};

enum DebugCmd : uint8_t {
    DEBUG_LOG = 0x00,
    DEBUG_PID = 0x01,
    DEBUG_SENSOR_RAW = 0x02,
    DEBUG_COMMAND_ECHO = 0x03,
};

/// Legacy 8-byte packet (pre-v0.2 ESP bridge); kept for reference
struct CommandPacket {
    uint8_t deviceID;
    uint8_t command;
    uint8_t type;
    int16_t value1;
    int16_t value2;
};

/// @deprecated Use LoRaPktType / DriveCmd — retained for existing driveCMDs stubs
enum MoveCMDType : uint8_t {
    STOP_Command = 0x01,
    DIR_FORWARD = 0x02,
    DIR_BACKWARD = 0x03,
    DIR_TURN_RIGHT = 0x04,
    DIR_TURN_LEFT = 0x05,
    DIR_TANK = 0x06,
    MAX_VEL = 0x07,
    TRIM_VAL = 0x08,
};

inline uint8_t loraPktTypeNibble(uint8_t pktType) {
    return (pktType >= 0x80) ? static_cast<uint8_t>((pktType >> 4) & 0x0F) : static_cast<uint8_t>(pktType & 0x0F);
}

inline uint8_t loraMakeHeader(uint8_t pktType, uint8_t deviceId) {
    if (pktType >= 0x80) {
        return pktType;
    }
    return static_cast<uint8_t>((loraPktTypeNibble(pktType) << 4) | (deviceId & 0x0F));
}

inline uint8_t loraHeaderPktType(uint8_t header) {
    if (header >= 0x80) {
        return header;
    }
    return static_cast<uint8_t>((header >> 4) & 0x0F);
}

inline uint8_t loraHeaderDeviceId(uint8_t header) {
    return static_cast<uint8_t>(header & 0x0F);
}

inline uint8_t loraSeq(const LoRaFrame& frame) {
    return static_cast<uint8_t>(frame.seqFlags & 0x3F);
}

inline bool loraWantsAck(const LoRaFrame& frame) {
    return (frame.seqFlags & 0x80) != 0;
}

inline bool loraIsUrgent(const LoRaFrame& frame) {
    return (frame.seqFlags & 0x40) != 0;
}

uint8_t loraCrc8(const uint8_t* data, size_t len);
bool loraValidateFrame(const LoRaFrame& frame);
void loraSetCrc(LoRaFrame& frame);
