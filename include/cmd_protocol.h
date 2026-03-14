#pragma once

#include <cstdint> // For uint8_t and int16_t

// Enum for different commands
enum RobotCommandType : uint8_t {
    CSTOP_Command = 0x01,
    ESTOP_Command = 0x02,
    //command for moving the robot
    MOVE_Command = 0x03, 
    //command for controling the relays
    RELAY_Command = 0x04,
    //commands that can change settings or manually control a system
    SYTEM_Command = 0x05,
};

// Enum for movement cmd types
enum MoveCMDType : uint8_t {
    // Drive cmds
    STOP_Command = 0x01,
    DIR_FORWARD = 0x02,
    DIR_BACKWARD = 0x03,
    // Turn cmds
    DIR_TURN_RIGHT = 0x04,
    DIR_TURN_LEFT = 0x05,
    // Tank drive
    DIR_TANK = 0x06,
    
};

// Enum for relay cmd types
enum RelayCMDType : uint8_t {
    ALL_Closed = 0x01,
    ALL_Open = 0x02,
    Relay_Num = 0x03,
};


// Structure for a command packet
struct CommandPacket {
    uint8_t deviceID;
    uint8_t command;
    uint8_t type;
    int16_t value1;
    int16_t value2;
};

struct LoRaFrame {
    uint8_t  magic;       // Always 0xA5 - quick garbage filter
    uint8_t  pktType;     // 0x01=Command, 0x81=Response/Ack, 0x02=Periodic status, 0x03=Heartbeat
    uint8_t  deviceID;    // 0x11 - your robot/base ID
    uint8_t  seq;         // 0-255 rolling sequence number
    uint16_t cmdStatus;   // 16-bit command or status code (expandable)
    int16_t  value1;      // Signed primary value (distance, speed, heading, etc.)
    int16_t  value2;      // Signed secondary value (tank right %, battery %, etc.)
    uint8_t  flags;       // Bitfield: bit0=wants ACK, bit1=urgent/live, etc.
    uint8_t  crc;         // CRC-8 over bytes 0-10
};