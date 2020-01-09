#pragma once

#ifndef TRUDPPP_CALLBACKS_HPP
#define TRUDPPP_CALLBACKS_HPP

#include <cstdint>
#include <functional>
#include <vector>

namespace trudppp {
    // Forward declaration of classes used in callbacks.
    class Packet;
    class Channel;
    class Connection;

    class Callbacks {
    public:
        std::function<void(Connection&)> connection_initialized;

        std::function<void(Connection&)> connection_destroyed;

        std::function<void(Connection&, Channel&, const Packet&)> packet_received;

        std::function<void(Connection&, Channel&, const std::vector<uint8_t>&)> unreliable_data_received;
    };
} // namespace trudppp

#endif
