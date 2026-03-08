#pragma once
#define Phoenix_No_WPI // remove WPI dependencies
#include <ctre/Phoenix.h>
#include "ctre/phoenix/platform/Platform.hpp"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include "ctre/phoenix/cci/Unmanaged_CCI.h"

class drivetrain {
    public:

    drivetrain(int leftID, int rightID, std::string interface);

    drivetrain();

    double getBusVoltage (int DeviceID);

    double getCurrent (int DeviceID);

    void Ddrive(double right, double left);

    private:
    ctre::phoenix::motorcontrol::can::TalonSRX talLeft;
    ctre::phoenix::motorcontrol::can::TalonSRX talRght;
    void initDrive();
};