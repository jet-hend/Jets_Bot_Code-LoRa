#pragma once

#include <cstdint> // For uint8_t and int16_t

// Enum for different commands
enum RobotCommands : uint8_t {
    MOVE_Command = 0x01,
    TURN_Command = 0x02,
    GYRO_Command = 0x03,
    STOP_Command = 0x04,
};

// Enum for movement types
enum DriveType : uint8_t {
    // Drive cmds
    DIR_FORWARD = 0x01,
    DIR_BACKWARD = 0x02,
    DIR_TANK = 0x03,
    // Turn cmds
    DIR_TURN_RIGHT = 0x01,
    DIR_TURN_LEFT = 0x02,
    // Gyro cmds
    RST_GYRO = 0x01,
};

// Structure for a command packet
struct CommandPacket {
    uint8_t deviceID;
    uint8_t command;
    uint8_t type;
    int16_t value1;
    int16_t value2;
};