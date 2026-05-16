#pragma once

#include "cmd_protocol.h"
#include "subsystems/drivetrain.h"
#include "subsystems/relayBoard.h"

class MPU6050;

class LoRaCommandHandler {
public:
    LoRaCommandHandler(drivetrain& drive, relayboard& relays, MPU6050& imu);

    /// @return false if frame should be ignored (wrong device, bad CRC, etc.)
    bool handleFrame(const LoRaFrame& frame);

    bool robotEnabled() const { return enabled_; }
    bool estopActive() const { return estop_; }

private:
    bool handleState(const LoRaFrame& frame);
    bool handleCommand(const LoRaFrame& frame);
    bool handleHeartbeat(const LoRaFrame& frame);
    bool handleTelemetry(const LoRaFrame& frame);
    bool handleError(const LoRaFrame& frame);
    bool handleDebug(const LoRaFrame& frame);

    void stopDrive();

    drivetrain& drive_;
    relayboard& relays_;
    MPU6050& imu_;
    bool enabled_ = true;
    bool estop_ = false;
};
