#include "driveCMDs.h"
#include <iostream>

DriveCommands::DriveCommands() {}

void DriveCommands::executeCommand(MoveCMDType cmdType) {
    switch (cmdType) {
        case STOP_Command:     stopMovement();   break;
        case DIR_FORWARD:      moveForward();    break;
        case DIR_BACKWARD:     moveBackward();   break;
        case DIR_TURN_LEFT:    turnLeft();       break;
        case DIR_TURN_RIGHT:   turnRight();      break;
        default:
            std::cout << "Unknown MoveCMDType: " << (int)cmdType << std::endl;
            break;
    }
}

void DriveCommands::moveForward()    { std::cout << "Moving Forward\n"; }
void DriveCommands::moveBackward()   { std::cout << "Moving Backward\n"; }
void DriveCommands::turnLeft()       { std::cout << "Turning Left\n"; }
void DriveCommands::turnRight()      { std::cout << "Turning Right\n"; }
void DriveCommands::stopMovement()   { std::cout << "Stopping Movement\n"; }