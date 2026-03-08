#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <linux/can.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>

// Forward declaration for types used in the implementation
#include <unistd.h>

class customCANutil {
public:
    customCANutil(const std::string& can_interface_name);
    ~customCANutil();

    bool sendCanFrame(uint32_t can_id, const std::vector<uint8_t>& data);
    bool isConnected() const;
    bool readCanFrame(struct can_frame& frame);
    void printCanFrame(const struct can_frame& frame) const;
    bool readSpecCanFrame(uint32_t target_id, struct can_frame& frame, int timeout_ms);


private:
    int s_socket_fd = -1; // File descriptor for the CAN socket
};