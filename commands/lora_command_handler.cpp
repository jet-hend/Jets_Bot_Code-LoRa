#include "commands/lora_command_handler.h"
#include "subsystems/MPU6050util.h"

#include <iostream>

LoRaCommandHandler::LoRaCommandHandler(drivetrain& drive, relayboard& relays, MPU6050& imu)
    : drive_(drive), relays_(relays), imu_(imu) {}

void LoRaCommandHandler::stopDrive() {
    drive_.Ddrive(0.0, 0.0);
}

bool LoRaCommandHandler::handleFrame(const LoRaFrame& frame) {
    if (!loraValidateFrame(frame)) {
        std::cerr << "LoRa: invalid frame (magic/CRC)\n";
        return false;
    }

    if (loraHeaderDeviceId(frame.header) != LORA_DEVICE_ID) {
        return false;
    }

    const uint8_t pktType = loraHeaderPktType(frame.header);
    if (pktType == PKT_STATE || pktType == 0x01) {
        return handleState(frame);
    }
    if (pktType == PKT_COMMAND || pktType == 0x02) {
        return handleCommand(frame);
    }
    if (pktType == PKT_HEARTBEAT || pktType == 0x03) {
        return handleHeartbeat(frame);
    }
    if (pktType == PKT_TELEMETRY || pktType == 0x04) {
        return handleTelemetry(frame);
    }
    if (pktType == PKT_ERROR || pktType == 0x05) {
        return handleError(frame);
    }
    if (pktType == PKT_DEBUG || pktType == 0x06) {
        return handleDebug(frame);
    }
    if (frame.header == PKT_RESPONSE) {
        std::cout << "LoRa: response pkt seq=" << static_cast<int>(loraSeq(frame)) << "\n";
        return true;
    }

    std::cerr << "LoRa: unknown pkt type 0x" << std::hex << static_cast<int>(pktType) << std::dec << "\n";
    return false;
}

bool LoRaCommandHandler::handleState(const LoRaFrame& frame) {
    switch (frame.command) {
        case STATE_ESTOP:
            estop_ = true;
            enabled_ = false;
            stopDrive();
            std::cout << "LoRa: ESTOP\n";
            break;
        case STATE_CSTOP:
            stopDrive();
            std::cout << "LoRa: controlled stop\n";
            break;
        case STATE_DISABLED:
            enabled_ = false;
            stopDrive();
            std::cout << "LoRa: disabled\n";
            break;
        case STATE_ENABLED:
            estop_ = false;
            enabled_ = true;
            std::cout << "LoRa: enabled\n";
            break;
        default:
            std::cerr << "LoRa: unknown state cmd 0x" << std::hex << static_cast<int>(frame.command)
                      << std::dec << "\n";
            return false;
    }
    return true;
}

bool LoRaCommandHandler::handleCommand(const LoRaFrame& frame) {
    if (!enabled_ || estop_) {
        std::cout << "LoRa: command ignored (disabled/estop)\n";
        return true;
    }

    switch (frame.command) {
        case DRIVE_STOP:
            stopDrive();
            break;
        case DRIVE_TANK: {
            const double left = static_cast<double>(frame.value1) / 100.0;
            const double right = static_cast<double>(frame.value2) / 100.0;
            drive_.Ddrive(right, left);
            break;
        }
        case DRIVE_ARC:
        case DRIVE_HEADING:
            std::cout << "LoRa: drive cmd 0x" << std::hex << static_cast<int>(frame.command) << " v1="
                      << frame.value1 << " v2=" << frame.value2 << std::dec << " (not implemented)\n";
            break;
        case DRIVE_SET_BREAKMODE:
        case DRIVE_SET_SOLBREAKS:
            std::cout << "LoRa: brake cmd 0x" << std::hex << static_cast<int>(frame.command) << std::dec
                      << " (not implemented)\n";
            break;
        case DRIVE_MAX_VELOCITY:
            std::cout << "LoRa: max velocity set to " << frame.value1 << "\n";
            break;
        case DRIVE_TRIM:
            std::cout << "LoRa: trim L=" << frame.value1 << " R=" << frame.value2 << "\n";
            break;
        case STM_REBOOT:
            std::cout << "LoRa: radio reboot requested\n";
            break;
        case STM_FAULTS_CLEAR:
            std::cout << "LoRa: fault clear\n";
            break;
        case STM_GYRO_CALI:
            imu_.getOffsets(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
            std::cout << "LoRa: gyro calibration\n";
            break;
        case STM_REQ_ERROR:
            std::cout << "LoRa: error status request\n";
            break;
        case STM_SET_DRIVE_PID:
            std::cout << "LoRa: PID set p=" << frame.value1 << " i/d=" << frame.value2 << "\n";
            break;
        default:
            std::cerr << "LoRa: unknown drive cmd 0x" << std::hex << static_cast<int>(frame.command)
                      << std::dec << "\n";
            return false;
    }
    return true;
}

bool LoRaCommandHandler::handleHeartbeat(const LoRaFrame& frame) {
    std::cout << "LoRa: heartbeat cmd=0x" << std::hex << static_cast<int>(frame.command) << " seq="
              << static_cast<int>(loraSeq(frame)) << std::dec << "\n";
    return true;
}

bool LoRaCommandHandler::handleTelemetry(const LoRaFrame& frame) {
    std::cout << "LoRa: telemetry cmd=0x" << std::hex << static_cast<int>(frame.command) << std::dec
              << " (reply not wired)\n";
    return true;
}

bool LoRaCommandHandler::handleError(const LoRaFrame& frame) {
    std::cout << "LoRa: error report cmd=0x" << std::hex << static_cast<int>(frame.command) << std::dec
              << "\n";
    if (frame.command == ERROR_CLEAR) {
        estop_ = false;
    }
    return true;
}

bool LoRaCommandHandler::handleDebug(const LoRaFrame& frame) {
    std::cout << "LoRa: debug cmd=0x" << std::hex << static_cast<int>(frame.command) << " v1=" << frame.value1
              << " v2=" << frame.value2 << std::dec << "\n";
    return true;
}
