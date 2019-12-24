#pragma once

#ifndef TRUDPPP_PACKET_HPP
#define TRUDPPP_PACKET_HPP

#include <chrono>
#include <cstdint>
#include <vector>

namespace trudppp {
    /// Trudppp packet type.
    enum class PacketType {
        Data = 0, ///< Packets with payload data.
        Ack = 1, ///< Packet confirming that certain sent packets was received.
        Reset = 2, ///< Packet initiating connection reset process.
        AckOnReset = 3, ///< Packet with acknowledgment that @a Reset packet was received.
        Ping = 4, ///< Ping packet.
        AckOnPing = 5, ///< Pong packet.
    };
} // namespace trudppp

#endif
