#pragma once

#include "subsystems/drivetrain.h"
#include "subsystems/ESPRaido.h"
#include "subsystems/relayBoard.h"
#include "cmd_protocol.h"

class DirDrive {
public:
    void DirDriveESP(drivetrain& robot_drivetrain, const CommandPacket& packet);
};