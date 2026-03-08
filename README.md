# 
Library binaries here
- [Raspberry PI (linux-armhf) ](lib/raspberry)
- [Linux Desktop (linux-amd64)](lib/x86-64)
- [Jetson TX2 (linux-aarch64)](lib/jetsontx)

This is the requisite platform interface for the integrator to provide for other platforms.
- [include/ctre/phoenix/Platform/Platform.h](include/ctre/phoenix/Platform/Platform.h)

If you link in phoenix-can-utils, you don't need to implement   CANComm_* , just implement   CANbus_*.  Conversely you can just implement the mid-level and don't bother with the low-level.
- [include/ctre/phoenix/Platform/Platform.h#L27](include/ctre/phoenix/Platform/Platform.h#L27)

#### CAN USB Adapter
SocketCAN USB adapter used below.
- Firmware : https://github.com/HubertD/candleLight_fw
- Hardware : http://canable.io/

Or alternatively deploy the SocketCAN firmware to a HERO
- Firmware : https://github.com/CrossTheRoadElec/HERO-STM32F4
- Hardware : http://www.ctr-electronics.com/control-system/hro.html

### Using Raspberry PI to control your robot

### Materials needed:
 - Raspberry Pi (3B+)
 - Micro SD card
 - CANable with CandleLight Firmware (https://canable.io/updater/ update here if not already done)
 - Laptop
 - Raspbian Buster with desktop (https://www.raspberrypi.org/downloads/raspbian/)

### Procedure:
 1. Flash SD card with Raspbian Desktop image. (see https://www.raspberrypi.org/documentation/installation/installing-images/README.md)  
    NOTE: Phoenix libraries require Debian **Bullseye** or later.
 2. Boot your Pi and connect to a Wi-Fi network (if you'd like to use Tuner on a windows PC make sure your windows PC and Raspberry Pi is connected to the same network.
 3. Continue with Software Setup.

# Jetson Nano setup
### Materials needed:
 - Jetson Nano
 - Micro SD card
 - CANable with CandleLight Firmware (https://canable.io/updater/ update here if not already done)
 - Laptop  
1. Setup Jetson Nano using instructions from Nvidia. 
https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit#intro  
2. Continue with Software Setup.

# Software Setup: 
1. Once you have your Device setup open a terminal and run the following commands to install necessary files.  
     -  `sudo apt-get upgrade`  
     -  `sudo apt-get update`    
     -  `sudo apt-get upgrade`  
2.	Install CAN tools `sudo apt-get install can-utils`.
3.	Install git `sudo apt-get install git`.  
4.	Install necessary libs to build example.   
     -  `sudo apt-get install cmake`  
     -  `sudo apt-get install libsdl2-dev`    
5. Clone repo into user directory `git clone https://github.com/CrossTheRoadElec/Phoenix-Linux-SocketCAN-Example.git`.  
6. Navigate into repo `cd ./Phoenix-Linux-SocketCAN-Example/.`
7.	Chmod shell scripts to allow you to use them:  
     -  `chmod +x build.sh`  
     -  `chmod +x clean.sh`  
     -  `chmod +x canableStart.sh`  
8.	Bring up can 0 `./canableStart.sh` (if you see the message `Device or resource busy` it means the can network is already up and requires no further action).   
     
# Validating SocketCan functionality 
1. Make sure you have talons or another CTRE CAN device connected for validation of can network.
2. Use `ifconfig` to display status of the CAN socket.
3. The first network listed should be can0 and should look like this
![can0](https://user-images.githubusercontent.com/51933047/63381988-c6733e80-c367-11e9-91fc-7e2f620fea02.png).  
4. Type `cansend can0 999#DEADBEEF` to send a CAN frame, your talons should now blink orange since a valid CAN message has been seen.
5. Use `candump can0` to see all incoming CAN traffic, which should display all periodic information being sent by a Talon.  
6. You should see a constant stream of messages similar to this:![candump](https://user-images.githubusercontent.com/51933047/63384109-2f5cb580-c36c-11e9-8688-d3fa774eab43.png)
7. To end the stream press `Ctrl+z`.

## Set up hot swapping
1. adding files for hot swapping compatibility 
2. Open a new terminal
3. Type `cd /etc/network/.`
4. Type `sudo gedit interfaces`
5. Copy the following lines into the file and click save  
    `allow-hotplug can0`    
    `iface can0 can static`     
    `bitrate 1000000`    
    `up /sbin/ip link set $IFACE down`    
    `up /sbin/ip link set $IFACE up type can`    
    Your file should look like this when finished.
![sudo](https://user-images.githubusercontent.com/51933047/63291621-e9332380-c291-11e9-8eac-91f53e9e89ce.png)
6.	When saving you may get a warning in your terminal; this is expected and not an issue.
7. Type `cd`.

# Running Socket Can Example: 
1.	Run Build.sh with `./build.sh`.
2.	Run program with `./run.sh`.
3.	You're now running Phoenix on your device. Confirm there are no error messages being sent to console output.
4.	You can stop your Program with `Ctrl+z`.