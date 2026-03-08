#include "commands/DirDrive.h"
#include "subsystems/drivetrain.h"
#include "cmd_protocol.h"

void DirDrive::DirDriveESP(drivetrain& robot_drivetrain, const CommandPacket& packet) {
    double speed1 = static_cast<double>(packet.value1) / 100;
    double speed2 = static_cast<double>(packet.value2) / 100;

    switch (packet.command) {
        case MOVE_Command: {
            if (packet.type == DIR_TANK) {
                std::cout << "Executing MOVE_Command: Tank at speeds " << speed1 << speed2 << std::endl;
                robot_drivetrain.Ddrive(speed1, speed2);
            } else {
                std::cerr << "Unknown type for MOVE_Command: 0x" << std::hex << (int)packet.type << std::endl;
            }
            break;
        }
        case TURN_Command: {
            if (packet.type == DIR_TURN_RIGHT) {
                std::cout << "Executing TURN_Command: Right at speed (placeholder)"<< std::endl;
                //robot_drivetrain.Ddrive(speed, -speed);
            } else if (packet.type == DIR_TURN_LEFT) {
                std::cout << "Executing TURN_Command: Left at speed (placeholder)"<< std::endl;
                //robot_drivetrain.Ddrive(-speed, speed);
            } else {
                std::cerr << "Unknown type for TURN_Command: 0x" << std::hex << (int)packet.type << std::endl;
            }
            break;
        }
        case STOP_Command: {
            std::cout << "Executing STOP_Command" << std::endl;
            robot_drivetrain.Ddrive(0, 0);
            break;
        }
        case GYRO_Command: {
            // This command would require additional methods in the drivetrain class.
            // For now, we will simply acknowledge the command.
            if (packet.type == RST_GYRO) {
                std::cout << "Executing GYRO_Command: Resetting Gyro (placeholder)" << std::endl;
                // robot_drivetrain.resetGyro();
            }
            break;
        }
        default:
            //std::cerr << "Unknown command received: 0x" << std::hex << (int)packet.command << std::endl;
            break;
    }
}