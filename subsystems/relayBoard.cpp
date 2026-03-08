#include "subsystems/relayBoard.h"
#include <wiringPi.h>

// Define the GPIO pins for each relay using the wiringPi numbering scheme.
const int RELAY_PIN_ONE   = 21; // BCM GPIO 5
const int RELAY_PIN_TWO   = 22; // BCM GPIO 6
const int RELAY_PIN_THREE = 26; // BCM GPIO 12
const int RELAY_PIN_FOUR    = 23; // BCM GPIO 13

// --- Class Constructor Implementation ---
// The constructor's name must match the class name and have no return type.
relayboard::relayboard() {
    wiringPiSetup();

    relays[static_cast<int>(relayNumber::RELAY_ONE)].gpioPin = RELAY_PIN_ONE;
    relays[static_cast<int>(relayNumber::RELAY_ONE)].state = false;
    pinMode(relays[static_cast<int>(relayNumber::RELAY_ONE)].gpioPin, OUTPUT);
    digitalWrite(relays[static_cast<int>(relayNumber::RELAY_ONE)].gpioPin, LOW);

    relays[static_cast<int>(relayNumber::RELAY_TWO)].gpioPin = RELAY_PIN_TWO;
    relays[static_cast<int>(relayNumber::RELAY_TWO)].state = false;
    pinMode(relays[static_cast<int>(relayNumber::RELAY_TWO)].gpioPin, OUTPUT);
    digitalWrite(relays[static_cast<int>(relayNumber::RELAY_TWO)].gpioPin, LOW);

    relays[static_cast<int>(relayNumber::RELAY_THREE)].gpioPin = RELAY_PIN_THREE;
    relays[static_cast<int>(relayNumber::RELAY_THREE)].state = false;
    pinMode(relays[static_cast<int>(relayNumber::RELAY_THREE)].gpioPin, OUTPUT);
    digitalWrite(relays[static_cast<int>(relayNumber::RELAY_THREE)].gpioPin, LOW);

    relays[static_cast<int>(relayNumber::RELAY_FOUR)].gpioPin = RELAY_PIN_FOUR;
    relays[static_cast<int>(relayNumber::RELAY_FOUR)].state = false;
    pinMode(relays[static_cast<int>(relayNumber::RELAY_FOUR)].gpioPin, OUTPUT);
    digitalWrite(relays[static_cast<int>(relayNumber::RELAY_FOUR)].gpioPin, LOW);
}

// NOTE: The global boolean variables have been removed.

// --- setRelay Method Implementation ---
void relayboard::setRelay(relayNumber relayID, bool state) {
    int index = static_cast<int>(relayID);

    if (index >= 0 && index < 4) {
        relays[index].state = state;
        digitalWrite(relays[index].gpioPin, state ? HIGH : LOW);
    }
}

// --- getRelay Method Implementation ---
bool relayboard::getRelay(relayNumber relayID) {
    int index = static_cast<int>(relayID);

    if (index >= 0 && index < 4) {
        return relays[index].state;
    }
    // FIX: A return statement is needed here for invalid IDs.
    return false;
}

void relayboard::setAllRelays(bool state) {
    setRelay(relayNumber::RELAY_ONE, state);
    setRelay(relayNumber::RELAY_TWO, state);
    setRelay(relayNumber::RELAY_THREE, state);
    setRelay(relayNumber::RELAY_FOUR, state);
}