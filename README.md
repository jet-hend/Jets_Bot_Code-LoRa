Jets_Bot_Code-LoRa
==================

# Long-range LoRa remote control for a home-built robot.

## Dependencies (RPi)
`sudo apt install git cmake build-essential can-utils libboost-all-dev -y`

## Setup Instructions (Raspberry Pi 4)
1. Clone repo
   `git clone https://github.com/jet-hend/Jets_Bot_Code-LoRa.git`
   `cd Jets_Bot_Code-LoRa`

2. Bring up CAN interface (USB-CAN adapter connected)
   `sudo ip link set can0 down`
   `sudo ip link set can0 up type can bitrate 1000000`

   Alternative: 'sudo ./canableStart.sh' (if present)
   Verify: 'ip link show can0'   → should show state UP
   Test:   'candump can0'        → expect periodic motor controller frames

3. `Build`
   `mkdir build && cd build`
   `cmake ..`
   `make -j4`

4. Run
   `sudo ./robot`
   Listens for packets from receiver ESP32 on /dev/ttyUSB*

## ESP32 Upload (Arduino IDE)
--------------------------
Board: Heltec WiFi LoRa 32(V3) or ESP32S3 Dev Module
- Upload base_test_basestationcodeV2_load_test.ino → Basestation-connected Heltec
- Upload Receiver_testV1.ino → robot Heltec
Baud: 115200, LoRa: 915 MHz (SF7, BW 125 kHz) – edit .ino if region differs

### Next Priorities
---------------
- Bidirectional comms: RPi → response packets → receiver ESP32 LoRa TX → base
- Simple command scheduler on RPi
- Periodic telemetry (heading, encoder pos, motor currents, voltage)
- Fixed-length protocol (seq # + CRC)
- Connection confirm, command ACK/status, basic debug

### Notes
-----
- All serial: 115200 8N1
- CAN: 1 Mbps
- Typical receiver port: /dev/ttyUSB0
- Phoenix 5 Linux libs included in lib/raspberry/

Jett (@Jet_hend) – early stage, feedback / PRs welcome!