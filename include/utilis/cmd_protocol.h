#pragma once

#include <cstdint> // For uint8_t and int16_t

/*
-----------------------------------------------------------------------------
|   Magic   |   Headder |   Seq + Flags |   Command |   Payload |   CRC-8   |
-----------------------------------------------------------------------------
|   0xA5    |   1 byte  |   1 bytes     |   1 bytes |   4 bytes |   1 bytes |
-----------------------------------------------------------------------------
PKT_STATE = 0x01
0x00	Disables high power drives, enables all breaks and sets all modes to idle
0x01	Enables all brakes and puts all modes to idle and exits the program
0x02	puts all modes to idle and disables power to drives
0x03	enables power to drives and starts main program
----------------------------------------------------------------------------
PKT_TELEOP = 0x02
0x00	STOP all Drives
0x01	Direct tank drive setting the Velocity of the right and left drivetrain motors
0x02	DRIVE FORWARD with encoder and heading assistance
0x03	DRIVE BACKWARD with encoder and heading assistance
0x04	SET the state a motor drive Break
0x05	SET the state a motor solenoid Break
0x06	SET the MAX velocity of the drive motors
0x07	SET the TRIM value for the drivetrain to allow for the bot to drive straight, but this should be mitigated by the installed encoders via vel or pos ctrl
0x08	REBOOTS the raspberry PI
0x09	CLEARS the current POS data on bot and sets the drive encoders to zero
0x10	START the gyro calibration sequence
0x11	REQ the errors from the PI
0x12	SET the PID values for a drive axis
----------------------------------------------------------------------------
PKT_HEARTBEAT = 0x03
0x00	Periodic Heartbeat message
0x01	Extended Heartbeat message
0x02	system and fault status
0x03	HB REQ
----------------------------------------------------------------------------
PKT_TELEMETRY = 0x04
0x00	Drivetrain Telemitry
0x01	Position and Heading
0x02	Battery Status
0x03	Gyro + Accelerometer
0x04	All key data in one packet
0x05	Request specific telemetry
----------------------------------------------------------------------------
PKT_ERROR = 0x05
0x00	Reports General fault
0x01	Reports Drive Faults
0x02	Sensor Failure 
0x03	Comms/LoRa Failure
0x04	Clear specific error
----------------------------------------------------------------------------
PKT_DEBUG = 0x06
0x00	Text Log MSG
0x01	PID Tuning Feedback
0x02	Raw Sensor Values
0x03	Echo the last received command
----------------------------------------------------------------------------
*/

// Enum for different packet types
enum class PacketTypes : uint8_t {
    PKT_STATE = 0x01,
    PKT_TELEOP = 0x02,
    PKT_HEARTBEAT = 0x03,
    PKT_TELEMETRY = 0x04,
    PKT_ERROR = 0x05,
    PKT_DEBUG = 0x06,
    PKT_RESPONSE = 0x07,
};

// Enum for different bot states
enum class STATE_PKT_CMDs : uint8_t {
    ESTOP = 0x00,
    CSTOP = 0x01,
    DISABLED = 0x02,
    ENABLED	= 0x03,
};

// Enum for Tele operation commands
enum class TELEOP_PKT_CMDs: uint8_t {
    DRIVE_STOP = 0x00,
    DRIVE_TANK = 0x01,
    DRIVE_ARC = 0x02,
    DRIVE_HEADING = 0x03,
    DRIVE_SET_BRAKEMODE = 0x04,
    DRIVE_SET_SOLBRAKES = 0x05,
    DRIVE_MAX_VELOCITY = 0x06,
    DRIVE_TRIM = 0x07,
    PI_REBOOT = 0x08,
    STM_FAULTS_CLEAR = 0x09,
    STM_GYRO_CALI = 0x10,
    STM_REQ_ERROR = 0x11,
    STM_SET_DRIVE_PID = 0x12,
};

enum class HEARTBEAT_PKT_CMDs : uint8_t {
    HEARTBEAT = 0x00,
    HEARTBEAT_EXT = 0x01,
    HEARTBEAT_STATUS = 0x02,
    HEARTBEAT_REQ = 0x03,
};

enum class TELEMETRY_PKT_CMDs : uint8_t {
    TELEM_DRIVE = 0x00,
    TELEM_ODOMETRY = 0x01,
    TELEM_BATTERY = 0x02,
    TELEM_IMU = 0x03,
    TELEM_FULL = 0x04,
    TELEM_REQ = 0x05,
};

enum class ERROR_PKT_CMDs : uint8_t {
    ERROR_FAULT = 0x00,
    ERROR_MOTOR = 0x01,
    ERROR_SENSOR = 0x02,
    ERROR_COMMS = 0x03,
    ERROR_CLEAR = 0x04,
};

enum class DEBUG_PKT_CMDs : uint8_t {
    DEBUG_LOG = 0x00,
    DEBUG_PID = 0x01,
    DEBUG_SENSOR_RAW = 0x02,
    DEBUG_COMMAND_ECHO = 0x03,
};

// Structure for the packet format
#pragma pack(push, 1)
struct PacketStructure {
    uint8_t magic = 0xA5; // Sync byte
    
    // Header byte split into two 4-bit fields
    // (Note: C++ packs low-order bits first on Little-Endian systems like ESP32/Pi)
    struct {
        uint8_t deviceID : 4; // Lower 4 bits (0-15)
        uint8_t pktType  : 4; // Upper 4 bits (Maps to PacketTypes)
    } header;

    // Seq + Flags byte split into 6-bit and 1-bit fields
    struct {
        uint8_t seq      : 6; // Lower 6 bits: Sequence counter (0-63)
        uint8_t urgent   : 1; // Bit 6: High priority flag
        uint8_t ack_req  : 1; // Bit 7: Request acknowledgment
    } seqFlags;

    uint8_t command; // Main command ID

    // Flexible 4-byte payload
    union {
        uint8_t  bytes[4];
        int16_t  int16s[2];
        uint32_t uint32_val;
        float    float_val;
    } payload;

    uint8_t crc8; // Checksum
};
static_assert(sizeof(PacketStructure) == 9, "PacketStructure size must be exactly 9 bytes!");
#pragma pack(pop)