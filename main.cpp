#include "commands/lora_command_handler.h"
#include "cmd_protocol.h"
#include "subsystems/ESPRaido.h"
#include "subsystems/MPU6050util.h"
#include "subsystems/customCANutil.h"
#include "subsystems/drivetrain.h"
#include "subsystems/relayBoard.h"

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

using boost::asio::io_context;

bool ENABLE = true;

void sleepApp(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
    io_context io;
    const std::string serial_port = "/dev/ttyUSB0";
    const unsigned int baud_rate = 921600;

    ESPRadio esp_radio(io, serial_port, baud_rate);
    std::cout << "Listening on " << serial_port << " at " << baud_rate << " baud (LoRa v0.2, "
              << LORA_FRAME_SIZE << "-byte frames)\n";

    c_SetPhoenixDiagnosticsStartTime(-1);

    drivetrain my_drivetrain;
    relayboard my_RelayBoard;
    customCANutil my_customCANutil("can0");

    MPU6050 my_MPU6050(0x68);
    float ax, ay, az, gr, gp, gy;
    my_MPU6050.calc_yaw = true;
    my_MPU6050.getOffsets(&ax, &ay, &az, &gr, &gp, &gy);

    LoRaCommandHandler command_handler(my_drivetrain, my_RelayBoard, my_MPU6050);

    while (ENABLE) {
        LoRaFrame frame{};
        if (esp_radio.readFrame(frame)) {
            printLoRaFrame(frame);
            command_handler.handleFrame(frame);
        }
        sleepApp(5);
    }

    return 0;
}
