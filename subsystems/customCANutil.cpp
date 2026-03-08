#include "subsystems/customCANutil.h"

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

customCANutil::customCANutil(const std::string& can_interface_name) {
    s_socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s_socket_fd < 0) {
        perror("Error creating CAN socket");
        return;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, can_interface_name.c_str());
    ioctl(s_socket_fd, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding CAN socket");
        close(s_socket_fd);
        s_socket_fd = -1;
    }
    
    fcntl(s_socket_fd, F_SETFL, O_NONBLOCK);
}

customCANutil::~customCANutil() {
    if (s_socket_fd >= 0) {
        close(s_socket_fd);
    }
}

bool customCANutil::sendCanFrame(uint32_t can_id, const std::vector<uint8_t>& data) {
    if (!isConnected()) {
        std::cerr << "CAN socket not connected." << std::endl;
        return false;
    }
    
    struct can_frame frame;
    frame.can_id = can_id;
    frame.can_dlc = data.size();
    
    if (data.size() > CAN_MAX_DLEN) {
        std::cerr << "Data payload too large." << std::endl;
        return false;
    }

    memcpy(frame.data, data.data(), frame.can_dlc);

    if (write(s_socket_fd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Error sending CAN frame");
        return false;
    }

    return true;
}

bool customCANutil::readCanFrame(struct can_frame& frame) {
    if (!isConnected()) {
        std::cerr << "CAN socket not connected." << std::endl;
        return false;
    }

    int bytes_read = read(s_socket_fd, &frame, sizeof(struct can_frame));

    if (bytes_read == sizeof(struct can_frame)) {
        return true;
    } else if (bytes_read == 0) {
        std::cerr << "CAN socket closed." << std::endl;
        return false;
    } else if (bytes_read == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return false;
        } else {
            perror("Error reading CAN frame");
            return false;
        }
    }
    return false;
}

bool customCANutil::readSpecCanFrame(uint32_t target_id, struct can_frame& frame, int timeout_ms) {
    auto start_time = std::chrono::high_resolution_clock::now();
    while (true) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        if (elapsed_time_ms >= timeout_ms) {
            return false; // Timeout expired
        }

        if (readCanFrame(frame)) {
            if (frame.can_id == target_id) {
                return true; // Match found
            }
        }
    }

}

bool customCANutil::isConnected() const {
    return s_socket_fd >= 0;
}

void customCANutil::printCanFrame(const struct can_frame& frame) const {
    std::cout << "ID: 0x" << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << frame.can_id;
    std::cout << " [" << std::dec << (int)frame.can_dlc << "] ";

    std::cout << std::hex << std::uppercase;
    for (int i = 0; i < frame.can_dlc; ++i) {
        std::cout << std::setw(2) << std::setfill('0') << (int)frame.data[i] << " ";
    }
    std::cout << std::endl;
}