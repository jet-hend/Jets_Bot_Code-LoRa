#include "subsystems/drivetrain.h"
#include "subsystems/ESPRadio.h"
#include "subsystems/customCANutil.h"
#include "subsystems/relayBoard.h"    // ← Add this
#include "subsystems/MPU6050util.h"
#include "cmd_protocol.h"

#include <boost/asio.hpp>             // ← Add this for io_context
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

using boost::asio::io_context;

bool ENABLE = true;
bool ERROR = false;
int ERROR_CODE = 0;

/** simple wrapper for code cleanup */
void sleepApp(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {

	boost::asio::io_context io_context;
	const std::string serial_port = "/dev/ttyUSB0"; // Assuming a USB-to-serial adapter
    const unsigned int baud_rate = 921600;

	ESPRadio esp_radio(io_context, serial_port, baud_rate);
	std::cout << "Listening on " << serial_port << " at " << baud_rate << " baud..." << std::endl;
	
	// Comment out the call if you would rather use the automatically running diag-server, note this requires uninstalling diagnostics from Tuner. 
	c_SetPhoenixDiagnosticsStartTime(-1); // disable diag server, instead we will use the diag server stand alone application that Tuner installs

	drivetrain my_drivetrain;

	relayboard my_RelayBoard;

	customCANutil my_customCANutil("can0");

	struct can_frame received_frame;

	MPU6050 my_MPU6050(0x68);
	float ax, ay, az, gr, gp, gy;
	float roll_angle, pitch_angle, yaw_angle;
	my_MPU6050.calc_yaw = true;
	my_MPU6050.getOffsets(&ax, &ay, &az, &gr, &gp, &gy);

	while (ENABLE == true){

	}
	
	return 0;
}