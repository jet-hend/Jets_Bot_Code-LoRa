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
	//std::cout << "Calculating the offsets...\n    Please keep the accelerometer level and still\n    This could take a couple of minutes...";
	//my_MPU6050.getOffsets(&ax, &ay, &az, &gr, &gp, &gy);
	//std::cout << "Gyroscope R,P,Y: " << gr << "," << gp << "," << gy << "\nAccelerometer X,Y,Z: " << ax << "," << ay << "," << az << "\n";

	my_MPU6050.calc_yaw = true;

	// sleepApp(2000);
	customCANutil my_customCANutil("can0");

	struct can_frame received_frame;

	boost::asio::io_context io_context;
	const std::string serial_port = "/dev/ttyUSB0"; // Assuming a USB-to-serial adapter
    const unsigned int baud_rate = 115200;

	ESPRadio esp_radio(io_context, serial_port, baud_rate);
	std::cout << "Listening on " << serial_port << " at " << baud_rate << " baud..." << std::endl;

	CommandPacket failsafe_packet = {0};
    CommandPacket received_packet = failsafe_packet;


	// //
	// const uint32_t node_id = 0x3f;
    // const uint32_t cmd_id = 0x06;

    // // --- Step 1: Calculate the base CAN ID ---
    // uint32_t base_can_id = (node_id << 5) | cmd_id;

    // // --- Step 2: Set the RTR bit ---
    // uint32_t final_can_id = base_can_id | CAN_RTR_FLAG;

    // // --- Step 3: Create an empty data vector for RTR ---
    // std::vector<uint8_t> data;

    // --- Step 4: Send the CAN frame ---
    // if (my_customCANutil.sendCanFrame(final_can_id, data)) {
    //     std::cout << "Successfully sent RTR frame." << std::endl;
    //     std::cout << "Final CAN ID (with RTR bit): 0x" << std::hex << final_can_id << std::endl;
    // } else {
    //     std::cerr << "Failed to send RTR frame." << std::endl;
    // }
	//

	while (ENABLE == true){

        CommandPacket temp_packet;
			
        if (esp_radio.readPacket(temp_packet)) {
			received_packet = temp_packet;

			ctre::phoenix::unmanaged::Unmanaged::FeedEnable(2000);
			std::cout << "FeedEnable status: " << ctre::phoenix::unmanaged::Unmanaged::GetEnableState() << std::endl;
            printPacket(received_packet);
        }

		if (!ctre::phoenix::unmanaged::Unmanaged::GetEnableState()) {
			my_RelayBoard.setAllRelays(false);
		}

		if (received_packet.command == MOVE_Command) {
			if (received_packet.value1 != 0 || received_packet.value2 != 0) {
				my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_ONE, true);
				my_dir_drive.DirDriveESP(my_drivetrain, received_packet);
				std::cout << "DEV0 CUR status: " << my_drivetrain.getCurrent(0) << std::endl;
				std::cout << "DEV1 CUR status: "<< my_drivetrain.getCurrent(1) << std::endl;
				std::cout << "DEV0 VOL status: " << my_drivetrain.getBusVoltage(0) << std::endl;
				std::cout << "DEV1 VOL status: "<< my_drivetrain.getBusVoltage(1) << std::endl;
			} else {
				my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_ONE, false);
			}
		}

		// my_MPU6050.getAccel(&ax, &ay, &az);
		// my_MPU6050.getGyro(&gr, &gp, &gy);
		// my_MPU6050.getAngle(0, &roll_angle);  // Get roll angle
        // my_MPU6050.getAngle(1, &pitch_angle); // Get pitch angle
        // my_MPU6050.getAngle(2, &yaw_angle);   // Get yaw angle
		
		// std::cout << "\r"
        //           << "Accel: X: " << ax << ", Y: " << ay << ", Z: " << az
        //           << " | Gyro: X: " << gr << ", Y: " << gp << ", Z: " << gy
		// 		  << " | Angle: Roll: " << roll_angle << ", Pitch: " << pitch_angle << ", Yaw: " << yaw_angle
        //           << std::flush;

		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_ONE, true);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_ONE, false);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_TWO, true);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_TWO, false);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_THREE, true);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_THREE, false);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_FOUR, true);
		// sleepApp(200);
		// my_RelayBoard.setRelay(relayboard::relayNumber::RELAY_FOUR, false);
		// sleepApp(200);


        
        // 4. Call the non-blocking readSpecificCanFrame function.
        // if (my_customCANutil.readCanFrame(received_frame)) {
        //     my_customCANutil.printCanFrame(received_frame);
        // }

		received_packet = failsafe_packet;
		sleepApp(20);
	}

	return 0;
}