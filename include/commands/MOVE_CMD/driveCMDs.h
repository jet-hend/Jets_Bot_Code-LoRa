#pragma once

#include "cmd_protocol.h"     // This brings in MoveCMDType

class DriveCommands {
public:
    DriveCommands();

    void executeCommand(MoveCMDType cmdType);

    void moveForward();
    void moveBackward();
    void turnLeft();
    void turnRight();
    void stopMovement();

private:
    // future private members
};