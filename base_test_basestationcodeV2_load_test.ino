#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RadioLib.h>

// Pin definitions for Heltec WiFi LoRa 32 (V3)
#define LORA_NSS 8
#define LORA_RST 12
#define LORA_DIO1 14
#define LORA_BUSY 13
#define LORA_SCK 9
#define LORA_MOSI 10
#define LORA_MISO 11

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define OLED_ADDR 0x3C
#define VEXT_PIN 36

#define LED_PIN 35

const uint8_t DEVICE_ID = 0x11;

const uint8_t MOVE_Command = 0x01;  // Linear movement command
const uint8_t TURN_Command = 0x02;  // Turn command
const uint8_t GYRO_Command = 0x03;  // Turn command
const uint8_t STOP_Command = 0x04;  // Stop command

// === type Definitions ===
//drive cmds
const uint8_t DIR_FORWARD = 0x01;
const uint8_t DIR_BACKWARD = 0x02;
const uint8_t DIR_TANK = 0x03;
//turn cmds
const uint8_t DIR_TURN_RIGHT = 0x01;
const uint8_t DIR_TURN_LEFT = 0x02;
//gyro cmds
const uint8_t RST_GYRO = 0x01;      //reset gyro

struct CommandPacket {
  uint8_t deviceID;
  uint8_t command;  // The command type (e.g., MOVE_Command, TURN_Command)
  uint8_t type;     // The type (e.g., DIR_FORWARD, DIR_TURN_LEFT)
  int16_t value1;   // The value associated with the command (e.g., distance, degrees)
  int16_t value2;   // The value associated with the command (e.g., distance, degrees)
};

//Creates SX1262 lora radio instance
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
//Creates SSD1306 OLED Display instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// === OLED helper function ===

const int MAX_HISTORY_MESSAGES = 6;
String messageHistory[MAX_HISTORY_MESSAGES];
int historyIndex = 0;

void updateDisplayStatus(const char* message) {
  static int scrollOffset = 0;
  static unsigned long lastScrollTime = 0;
  const unsigned long SCROLL_DELAY = 300;  // Delay in milliseconds per character scroll

  // If a new message is provided, update history and reset scrolling
  if (message != nullptr && message[0] != '\0') {
    // Add the new message to the history buffer
    if (historyIndex < MAX_HISTORY_MESSAGES) {
      messageHistory[historyIndex] = String(message);
      historyIndex++;
    } else {
      for (int i = 0; i < MAX_HISTORY_MESSAGES - 1; i++) {
        messageHistory[i] = messageHistory[i + 1];
      }
      messageHistory[MAX_HISTORY_MESSAGES - 1] = String(message);
    }
    scrollOffset = 0;  // Reset scroll on new message
  }

  // Handle horizontal scrolling for the entire display
  int visibleChars = SCREEN_WIDTH / 6;  // 6 is text size 1 character width

  // Find the longest string to determine the maximum scroll length
  int longestMessageLength = 0;
  for (int i = 0; i < historyIndex; i++) {
    if (messageHistory[i].length() > longestMessageLength) {
      longestMessageLength = messageHistory[i].length();
    }
  }

  if (millis() - lastScrollTime > SCROLL_DELAY) {
    if (longestMessageLength > visibleChars) {
      scrollOffset++;
      if (scrollOffset > longestMessageLength - visibleChars) {
        scrollOffset = 0;  // Reset scroll to beginning
      }
    } else {
      scrollOffset = 0;  // No need to scroll if all messages fit
    }
    lastScrollTime = millis();
  }

  // Clear the display to prepare for redrawing
  display.clearDisplay();

  // === Display Scrolling History ===
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display all history messages except the last one (which is the latest)
  int historyLines = min(historyIndex, MAX_HISTORY_MESSAGES - 1);
  for (int i = 0; i < historyLines; i++) {
    display.setCursor(0, i * 9);  // Each line is 9 pixels (8 text, 1 padding)
    String currentMessage = messageHistory[i];
    if (currentMessage.length() > visibleChars) {
      display.println(currentMessage.substring(scrollOffset, scrollOffset + visibleChars));
    } else {
      display.println(currentMessage);
    }
  }

  // === Display Latest Message at the bottom ===
  if (historyIndex > 0) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    // Position for the latest message on the last line
    int lastLineY = SCREEN_HEIGHT - 8;
    display.setCursor(0, lastLineY);

    String latestMessage = messageHistory[historyIndex - 1];
    if (latestMessage.length() > visibleChars) {
      display.println(latestMessage.substring(scrollOffset, scrollOffset + visibleChars));
    } else {
      display.println(latestMessage);
    }
  }

  // Push the buffer to the display
  display.display();
}

String waitForSerial();
bool parseSerialCommand(String serialCommand, CommandPacket& packet);
bool sendLoRaFrame(const CommandPacket& packet);

void setup() {

  pinMode(LED_PIN, OUTPUT);
  // Initializes the seiral port
  Serial.begin(115200);

  // Initialize Vext pin to power the OLED
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);  // Set LOW to enable OLED power
  delay(100);                   // Allow time for OLED to stabilize

  // Initialize I2C with specific SDA and SCL pins
  Wire.begin(OLED_SDA, OLED_SCL);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  // Clear the display buffer
  display.clearDisplay();

  // Configure display settings
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // White text on black background
  display.setCursor(0, 0);              // Start at top-left corner

  // Write text to the display
  updateDisplayStatus("Heltec ESP32-S3");
  updateDisplayStatus("Base station");

  Serial.print(F("[SX1262] Initializing: "));
  updateDisplayStatus("[SX1262] Init");

  int state = radio.begin(915.0);  // Set frequency to 915 MHz (adjust for your region, e.g., 868.0 for EU)

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
    updateDisplayStatus("[SX1262] success!");
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    updateDisplayStatus("Radio init failed");
    while (true) {};  // Halt if initialization fails
  }

  // Optional: Configure LoRa parameters
  radio.begin(915.0);
  radio.setBandwidth(125.0);    // Bandwidth: 125 kHz
  radio.setSpreadingFactor(7);  // Spreading Factor: 7
  radio.setCodingRate(5);       // Coding Rate: 4/5
  radio.setOutputPower(14);     // Output Power: 14 dBm (adjust as needed)
  radio.setPreambleLength(8);

  //radio.setPacketReceivedAction(setFlag);

  digitalWrite(LED_PIN, 1);
  // Update display with ready status
  updateDisplayStatus("Ready for commands");
  delay(2000);
  digitalWrite(LED_PIN, 0);
}

void loop() {
  // Check for incoming serial data from the phone app
  if (Serial.available() > 0) {
    // Wait for and read the full serial command
    String serialCommand = waitForSerial();
    Serial.print("Received command: ");
    Serial.println(serialCommand);

    // Create a command packet struct to store the parsed data
    CommandPacket packet;

    // Parse the serial command and populate the packet
    if (parseSerialCommand(serialCommand, packet)) {

      // String frameInfo = "Sent: ID=" + String(packet.deviceID, HEX) + " Cmd=" + String(packet.command, HEX) + " Dir=" + String(packet.type, HEX) + " Val1=" + String(packet.value1) + " Val2=" + String(packet.value2);
      // updateDisplayStatus(frameInfo.c_str());

      // If parsing was successful, send the packet over LoRa
      if (sendLoRaFrame(packet)) {
        String statusMessage = serialCommand + " -> Sent!";
        updateDisplayStatus(statusMessage.c_str());
        Serial.println("Command successfully sent via LoRa.");
      } else {
        String statusMessage = serialCommand + " -> Send failed!";
        updateDisplayStatus(statusMessage.c_str());
        Serial.println("Failed to send command via LoRa.");
      }
    } else {
      String statusMessage = serialCommand + " -> Invalid!";
      updateDisplayStatus(statusMessage.c_str());
      Serial.println("Error: Invalid serial command format.");
    }
  }

  // Continuously update the display to allow for scrolling animation
  updateDisplayStatus(nullptr);
}

/**
 * @brief Waits for and reads a complete line from the serial buffer.
 * A command is considered complete when a newline character ('\n') is received.
 * @return The complete serial command as a String.
 */
String waitForSerial() {
  String serialData = "";
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    serialData += incomingChar;
    if (incomingChar == '\n') {
      break;  // End of line character received
    }
  }
  serialData.trim();  // Remove leading/trailing whitespace
  return serialData;
}

/**
 * @brief Parses a serial command string and populates a CommandPacket struct.
 * Supports "F 10" for move forward, "T R 310" for turn right, and "T L 310" for turn left.
 * @param serialCommand The input string from the serial port.
 * @param packet The CommandPacket struct to be populated.
 * @return true if the command was successfully parsed, false otherwise.
 */
bool parseSerialCommand(String serialCommand, CommandPacket& packet) {
  packet.deviceID = DEVICE_ID;
  packet.command = 0x00;
  packet.type = 0x00;
  packet.value1 = 0x00;
  packet.value2 = 0x00;
  // Split the string into parts using ' ' as a delimiter
  int firstSpace = serialCommand.indexOf(' ');
  if (firstSpace == -1) {
    // Not a valid two or three part command, possibly a single character command
    if (serialCommand.equalsIgnoreCase("S")) {
      packet.deviceID = DEVICE_ID;
      packet.command = STOP_Command;
      return true;
    } else if (serialCommand.equalsIgnoreCase("G")) {
      packet.deviceID = DEVICE_ID;
      packet.command = GYRO_Command;
      packet.type = RST_GYRO;
      return true;
    }
    return false;
  }

  String commandPart = serialCommand.substring(0, firstSpace);
  String remaining = serialCommand.substring(firstSpace + 1);

  if (commandPart.equalsIgnoreCase("M")) {
    int secondSpace = remaining.indexOf(' ');
    String value1Part = remaining.substring(0, secondSpace);

    packet.command = MOVE_Command;
    // Correctly set direction based on the sign of the value
    if (value1Part.toInt() >= 0) {
      packet.type = DIR_FORWARD;
      packet.value1 = (int16_t)value1Part.toInt();
    } else {
      packet.type = DIR_BACKWARD;
      packet.value1 = (int16_t)value1Part.toInt();
    }
    return true;
  } else if (commandPart.equalsIgnoreCase("T")) {
    // Turn command: "T R 310" or "T L 310"
    int secondSpace = remaining.indexOf(' ');
    if (secondSpace == -1) {
      return false;  // Malformed turn command
    }
    String typePart = remaining.substring(0, secondSpace);
    String valuePart = remaining.substring(secondSpace + 1);
    int value = valuePart.toInt();

    if (typePart.equalsIgnoreCase("R") && value > 0) {
      packet.deviceID = DEVICE_ID;
      packet.command = TURN_Command;
      packet.type = DIR_TURN_RIGHT;
      packet.value1 = (int16_t)value;
      return true;
    } else if (typePart.equalsIgnoreCase("L") && value > 0) {
      packet.deviceID = DEVICE_ID;
      packet.command = TURN_Command;
      packet.type = DIR_TURN_LEFT;
      packet.value1 = (int16_t)value;
      return true;
    }
  } else if (commandPart.equalsIgnoreCase("TD")) {
    int secondSpace = remaining.indexOf(' ');
    if (secondSpace == -1) {
      return false; // Malformed TD command
    }
    String value1Part = remaining.substring(0, secondSpace);
    String value2Part = remaining.substring(secondSpace + 1);
    packet.command = MOVE_Command; // Re-using MOVE_Command
    packet.type = DIR_TANK;        // Using the new DIR_TANK type
    packet.value1 = (int16_t)value1Part.toInt();
    packet.value2 = (int16_t)value2Part.toInt();
    return true;
  }
  // Command not recognized or invalid format
  return false;
}

/**
 * @brief Sends the command packet over LoRa.
 * It transmits the raw bytes of the CommandPacket struct.
 * @param packet The CommandPacket struct to send.
 * @return true if the transmission was successful, false otherwise.
 */
bool sendLoRaFrame(const CommandPacket& packet) {
  // We send the struct as a raw byte array
  int state = radio.transmit((uint8_t*)&packet, sizeof(packet));
  if (state == RADIOLIB_ERR_NONE) {
    // The transmission was successful
    return true;
  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    // The transmission timed out
    Serial.println(F("transmit() failed, timeout!"));
    return false;
  } else {
    // Other error
    Serial.print(F("transmit() failed, code "));
    Serial.println(state);
    return false;
  }
}
