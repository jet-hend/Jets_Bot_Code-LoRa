#include "subsystems/drivetrain.h"
#include "subsystems/ESPRaido.h"
#include "subsystems/relayBoard.h"
#include "subsystems/customCANutil.h"
#include "subsystems/MPU6050util.h"
#include "commands/DirDrive.h"
#include "cmd_protocol.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

bool ENABLE = true;
bool ERROR = false;
int ERROR_CODE = 0;

/** simple wrapper for code cleanup */
void sleepApp(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
	// Comment out the call if you would rather use the automatically running diag-server, note this requires uninstalling diagnostics from Tuner. 
	c_SetPhoenixDiagnosticsStartTime(-1); // disable diag server, instead we will use the diag server stand alone application that Tuner installs

	drivetrain my_drivetrain;

	DirDrive my_dir_drive;

	relayboard my_RelayBoard;

	MPU6050 my_MPU6050(0x68);
	float ax, ay, az, gr, gp, gy;
	float roll_angle, pitch_angle, yaw_angle;

	//Calculate the offsets
	std::cout << "Calculating the offsets...\n    Please keep the accelerometer level and still\n    This could take a couple of minutes...";
	my_MPU6050.getOffsets(&ax, &ay, &az, &gr, &gp, &gy);
	std::cout << "Gyroscope R,P,Y: " << gr << "," << gp << "," << gy << "\nAccelerometer X,Y,Z: " << ax << "," << ay << "," << az << "\n";

	my_MPU6050.calc_yaw = true;

	customCANutil my_customCANutil("can0");

	struct can_frame received_frame;

	boost::asio::io_context io_context;
	const std::string serial_port = "/dev/ttyUSB0"; // Assuming a USB-to-serial adapter
    const unsigned int baud_rate = 921600;

	ESPRadio esp_radio(io_context, serial_port, baud_rate);
	std::cout << "Listening on " << serial_port << " at " << baud_rate << " baud..." << std::endl;

	CommandPacket failsafe_packet = {0};
    CommandPacket received_packet = failsafe_packet;

	while (ENABLE == true){

        CommandPacket temp_packet;
			
        if (esp_radio.readPacket(temp_packet)) {
			received_packet = temp_packet;
        }

		if (!ctre::phoenix::unmanaged::Unmanaged::GetEnableState()) {
			my_RelayBoard.setAllRelays(false);
		}

		received_packet = failsafe_packet;
		sleepApp(20);
	}

	return 0;
}