#pragma once

#include <cstdint> // For uint8_t and int16_t

// Enum for different commands
enum RobotCommandType : uint8_t {
    //command for doing a controlled stop of the robot, cutting power to the drives
    CSTOP_Command = 0x01,
    //command for Emergency stoping the robot, cutting power to drives.
    ESTOP_Command = 0x02,
    //command for moving the robot
    MOVE_Command = 0x03, 
    //command for controling the relays.
    RELAY_Command = 0x04,
    //commands that can change settings or manually control a system.
    SYSTEM_Command = 0x05,
    //commands that requests the status of the robot.
    INFO_REQUEST_Command = 0x06,
};

// Structure for a command packet
struct CommandPacket {
    uint8_t deviceID;
    uint8_t command;
    uint8_t type;
    int16_t value1;
    int16_t value2;
};

// Enum for movement cmd types
enum MoveCMDType : uint8_t {
    // Drive cmds
    //sets drive motors to 0 and breaks
    STOP_Command = 0x01,
    //sets the robot to drive a calculated distance Forward using the encoders and MPU6050
    DIR_FORWARD = 0x02,
    //sets the robot to drive a calculated distance Backward using the encoders and MPU6050
    DIR_BACKWARD = 0x03,
    //sets the robot to turn to the Right to a calculated angle using the encoders and MPU6050
    DIR_TURN_RIGHT = 0x04,
    //sets the robot to turn to the Left to a calculated angle using the encoders and MPU6050
    DIR_TURN_LEFT = 0x05,
    // Tank drive setting two values to set the left and right motors to
    DIR_TANK = 0x06,
    
};

// Enum for relay cmd types
// enum RelayCMDType : uint8_t {
//     //sets all relays to closed
//     ALL_Closed = 0x01,
//     //sets all relays to open
//     ALL_Open = 0x02,
//     //sets a single relay's state
//     Relay_Num = 0x03,
// };

// Enum for system cmd types
enum SystemCMDType : uint8_t {
    //
    RadioRST = 0x01,
    //
    CANBusRST = 0x02,
    //Clicks all 
    RelayCLKTest = 0x03,
    //Blinks both motor controllers to verify comms with CAN0
    MotorControllerBLINK = 0x04,
    //resets and calibrates the gyro
    GYRORST = 0x05,
    //resets the motor encoders to zero
    EncoderRST=0x06,
};

// Enum for packet types
enum PKT_TYPES : uint8_t {
    //command packet type for general functions
    PKT_COMMAND = 0x01,
    //Response/ACK packet type
    PKT_RESPONSE = 0x81,
    //packet type for the Heartbeat
    PKT_HEARTBEAT = 0x02,
    //packet type for collecting Tele data
    PKT_TELEMETRY = 0x03,
    //packet type for telemetry data
    PKT_ERROR = 0x04,
    //packet type for debug info
    PKT_DEBUG = 0x05,
    //packet type for Emergency stops
    PKT_ESTOP = 0x06
};

struct LoRaFrame {
    uint8_t  magic;       // Always 0xA5 - garbage filter
    uint8_t  pktType;     // 0x01=Command, 0x81=Response/Ack, 0x03=Periodic status, 0x02=Heartbeat
    uint8_t  deviceID;    // 0x11 - your robot/base ID
    uint8_t  seq;         // 0-255 rolling sequence number
    uint16_t cmdStatus;   // 16-bit command or status code (expandable)
    int16_t  value1;      // Signed primary value (distance, speed, heading, etc.)
    int16_t  value2;      // Signed secondary value (tank right %, battery %, etc.)
    uint8_t  flags;       // Bitfield: bit0=wants ACK, bit1=urgent/live, etc.
    uint8_t  crc;         // CRC-8 over bytes 0-10
};