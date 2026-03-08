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
//turn cmds
const uint8_t DIR_TURN_RIGHT = 0x01;
const uint8_t DIR_TURN_LEFT = 0x02;
//gyro cmds
const uint8_t RST_GYRO = 0x01;      //reset gyro
const uint8_t DIR_GYRO_PID = 0x03;  //hold headding of bot with gyro

struct CommandPacket {
  uint8_t deviceID;
  uint8_t command;  // The command type (e.g., MOVE_Command, TURN_Command)
  uint8_t type;     // The type (e.g., DIR_FORWARD, DIR_TURN_LEFT)
  uint16_t value;   // The value associated with the command (e.g., distance, degrees)
};

volatile bool receivedFlag = false; // Flag to indicate a packet has been received

void updateDisplayStatus(const char* message);
bool receiveLoRaPacket(CommandPacket& packet);
void parseAndDisplayPacket(const CommandPacket& packet);
void onReceive(void); // ISR (Interrupt Service Routine) for DIO1

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

  // Clear the display buffer
  display.clearDisplay();

  // Configure display settings
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // White text on black background
  display.setCursor(0, 0);              // Start at top-left corner

  // Write text to the display
  updateDisplayStatus("Heltec ESP32-S3");
  updateDisplayStatus("Base station");

  updateDisplayStatus("[SX1262] Init");
  int state = radio.begin(915.0);  // Set frequency to 915 MHz (adjust for your region, e.g., 868.0 for EU)

  if (state == RADIOLIB_ERR_NONE) {
    updateDisplayStatus("[SX1262] success!");
  } else {
    updateDisplayStatus("Radio init failed");
    while (true) {};  // Halt if initialization fails
  }

  // Optional: Configure LoRa parameters
  radio.setBandwidth(125.0);    // Bandwidth: 125 kHz
  radio.setSpreadingFactor(7);  // Spreading Factor: 7
  radio.setCodingRate(5);       // Coding Rate: 4/5
  radio.setOutputPower(14);     // Output Power: 14 dBm (adjust as needed)
  radio.setPreambleLength(8);
  digitalWrite(LED_PIN, 1);
  // Update display with ready status
  updateDisplayStatus("Ready for commands");
  delay(2000);
  digitalWrite(LED_PIN, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (receivedFlag) {
    digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate activity
    
    // Create a packet to hold the received data
    CommandPacket receivedPacket;

    // Attempt to read the packet from the LoRa module
    if (receiveLoRaPacket(receivedPacket)) {
      // If successful, parse and display the packet's contents
      parseAndDisplayPacket(receivedPacket);
    }

    // Reset the flag and put the radio back into receive mode
    receivedFlag = false;
    radio.startReceive();
    
    digitalWrite(LED_PIN, LOW); // Turn off LED
  }
  // The updateDisplayStatus function could be called here with nullptr
  // if you want the scrolling animation to run continuously.
  updateDisplayStatus(nullptr);
}

/**
 * @brief Reads the incoming data from the LoRa module into the CommandPacket struct.
 * @param packet A reference to the CommandPacket struct to fill with data.
 * @return True if a packet was read successfully, false otherwise.
 */
bool receiveLoRaPacket(CommandPacket& packet) {
  int state = radio.readData((uint8_t*)&packet, sizeof(CommandPacket));
  
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Packet received successfully!"));
    return true;
  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    Serial.println(F("CRC error!"));
    updateDisplayStatus("Receive Error: CRC");
    return false;
  } else {
    Serial.print(F("readData failed, code "));
    Serial.println(state);
    updateDisplayStatus("Receive Error");
    return false;
  }
}

/**
 * @brief Parses the data from a CommandPacket and displays it on the OLED and Serial monitor.
 * @param packet The CommandPacket containing the data to parse and display.
 */
void parseAndDisplayPacket(const CommandPacket& packet) {
  // Format the received data into a human-readable string
  String info = "Rcvd: ID=" + String(packet.deviceID, HEX) +
                " Cmd=" + String(packet.command, HEX) +
                " Typ=" + String(packet.type, HEX) +
                " Val=" + String(packet.value);

  // Print to Serial monitor for debugging
  Serial.println(info);
  
  // Send the same information to the Raspberry Pi via Serial
  // This is where the ESP32 forwards the command to the robot's main brain.
  Serial.println(info); 

  // Update the OLED display with the new information
  updateDisplayStatus(info.c_str());

  // Additionally, you could forward the raw packet to the Pi like this:
  // Serial.write((uint8_t*)&packet, sizeof(CommandPacket));
}

/**
 * @brief Interrupt Service Routine (ISR) for DIO1.
 * This function is called automatically when the LoRa module receives a packet.
 * It just sets a flag; all heavy processing is done in the main loop.
 */
void onReceive(void) {
  receivedFlag = true;
}