#pragma once
#include <wiringPi.h>

class relayboard {
    public:

    enum class relayNumber : int {
        // Drive cmds
        RELAY_ONE,
        RELAY_TWO,
        RELAY_THREE,
        RELAY_FOUR
    };

    relayboard();

    void setRelay(relayNumber relayID, bool state);

    bool getRelay(relayNumber relayID);

    void setAllRelays(bool state);

    private:
    struct relayState {
        // This struct could hold more information about each relay, like its current state.
        int gpioPin;
        bool state;
    };

    relayState relays[4];
};