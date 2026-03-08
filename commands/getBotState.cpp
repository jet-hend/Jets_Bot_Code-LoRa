#include "subsystems/drivetrain.cpp"

void sendFull() {
    sendDrivetrain();
    sendTurret();
}

void sendDrivetrain() {
    //get left
    getBusVoltage(0);
    getCurrent(0);
    
    //get right
    double lV = getBusVoltage(1);
    double lc = getCurrent(1);
    send_P("LMV:" + to_string(lV) + " LMC:"+ to_string(lc));
}

void sendTurret() {

}