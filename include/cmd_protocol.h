#pragma once

#include <cstdint> /// @brief  For uint8_t and int16_t

/// @brief  Enum for different commands
enum RobotCommandType : uint8_t {
    /// @brief command for doing a controlled stop of the robot, cutting power to the drives
    CSTOP_Command = 0x01,
    /// @brief command for Emergency stoping the robot, cutting power to drives.
    ESTOP_Command = 0x02,
    /// @brief command for moving the robot
    MOVE_Command = 0x03, 
    /// @brief command for controling the relays.
    RELAY_Command = 0x04,
    /// @brief commands that can change settings or manually control a system.
    SYSTEM_Command = 0x05,
    /// @brief commands that requests the status of the robot.
    AUTO_Command = 0x06,
    /// @brief commands that requests the radio to perform an action
    RADIO_Command = 0x07
};

/// @brief  Enum for movement cmd types
enum MoveCMDType : uint8_t {
    /// @brief  Drive cmds
    /// @brief sets drive motors to 0 and breaks
    STOP_Command = 0x01,
    /// @brief sets the robot to drive a calculated distance Forward using the encoders and MPU6050
    DIR_FORWARD = 0x02,
    /// @brief sets the robot to drive a calculated distance Backward using the encoders and MPU6050
    DIR_BACKWARD = 0x03,
    /// @brief sets the robot to turn to the Right to a calculated angle using the encoders and MPU6050
    DIR_TURN_RIGHT = 0x04,
    /// @brief sets the robot to turn to the Left to a calculated angle using the encoders and MPU6050
    DIR_TURN_LEFT = 0x05,
    /// @brief  Tank drive setting two values to set the left and right motors to
    DIR_TANK = 0x06,
    /// @brief set the global max Velocity
    MAX_VEL = 0x07,
    /// @brief set the trim value for the left and right motors ()
    TRIM_VAL = 0x08,
    
};

/// @brief  Enum for relay cmd types
enum RelayCMDType : uint8_t {
    /// @brief sets all relays to closed
    ALL_Closed = 0x01,
    /// @brief sets all relays to open
    ALL_Open = 0x02,
    /// @brief sets a single relay's state
    Relay_Num = 0x03,
};

/// @brief  Enum for system cmd types
enum SystemCMDType : uint8_t {
    /// @brief 
    RadioRST = 0x01,
    /// @brief 
    CANBusRST = 0x02,
    /// @brief Clicks all 
    RelayCLKTest = 0x03,
    /// @brief Blinks both motor controllers to verify comms with CAN0
    MotorControllerBLINK = 0x04,
    /// @brief resets and calibrates the gyro
    GYRORST = 0x05,
    /// @brief resets the motor encoders to zero
    EncoderRST=0x06,
};


/// @brief  Structure for a command packet
struct CommandPacket {
    uint8_t deviceID;
    uint8_t command;
    uint8_t type;
    int16_t value1;
    int16_t value2;
};

struct LoRaFrame {
    /// @brief  Always 0xA5 - quick garbage filter
    uint8_t  magic;       
    /// @brief  0x01=Command, 0x81=Response/Ack, 0x02=Periodic status, 0x03=Heartbeat
    uint8_t  pktType;     
    /// @brief  0x11 - your robot/base ID
    uint8_t  deviceID;    
    /// @brief  0-255 rolling sequence number
    uint8_t  seq;         
    /// @brief  16-bit command or status code (expandable)
    uint16_t cmdStatus;   
    /// @brief  Signed primary value (distance, speed, heading, etc.)
    int16_t  value1;      
    /// @brief  Signed secondary value (tank right %, battery %, etc.)
    int16_t  value2;      
    /// @brief  Bitfield: bit0=wants ACK, bit1=urgent/live, etc.
    uint8_t  flags;       
    /// @brief  CRC-8 over bytes 0-10
    uint8_t  crc;         
};