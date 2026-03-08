#include "subsystems/drivetrain.h"
#include <iostream>
#include <string>
#include <unistd.h>

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;
using namespace std;

/* make some talons for drive train */
drivetrain::drivetrain(int leftID, int rightID, std::string interface) 
    : talLeft(leftID, interface), talRght(rightID, interface) {
    // Now you can set the properties of the talons inside the constructor body.
    initDrive();
}

// A simple default constructor that uses some pre-defined values.
drivetrain::drivetrain() : drivetrain(1, 0, "can0") {}

void drivetrain::initDrive()
{
	talRght.SetInverted(true);
}

double drivetrain::getBusVoltage(int DeviceID) {
    if (DeviceID == 0) {
        return talRght.GetBusVoltage();
    } else if (DeviceID == 1) {
        return talLeft.GetBusVoltage();
    }
    return 0.0; // Return a default value if the ID is not found.
}

double drivetrain::getCurrent(int DeviceID) {
    if (DeviceID == 0) {
        return talRght.GetStatorCurrent();
    } else if (DeviceID == 1) {
        return talLeft.GetStatorCurrent();
    }
    return 0.0;
}

void drivetrain::Ddrive(double right, double left) {
    talLeft.Set(ControlMode::PercentOutput, left);
    talRght.Set(ControlMode::PercentOutput, right);
}