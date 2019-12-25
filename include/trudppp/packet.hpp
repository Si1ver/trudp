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

    /// Trudppp packet.
    class Packet {
    private:
        /// The type of the packet.
        PacketType type;

        /// Channel number.
        uint8_t channel_number;

        /// Packet identifier.
        uint32_t id;

        /// Payload data transmitted with packet.
        std::vector<uint8_t> data;

        /// The moment of time when the packet was sent.
        std::chrono::system_clock::time_point timestamp;

        Packet(const Packet&) = delete;

        Packet& operator=(const Packet&) = delete;

    public:
        Packet() : type(PacketType::Data), channel_number(0), id(0) {}

        Packet(PacketType type, uint8_t channel_number, uint32_t id, std::vector<uint8_t> data,
            std::chrono::system_clock::time_point timestamp)
            : type(type), channel_number(channel_number), id(id), data(data), timestamp(timestamp) {
        }

        Packet(Packet&& other) noexcept
            : type(other.type), channel_number(other.channel_number), id(other.id),
              data(std::move(other.data)), timestamp(other.timestamp) {}

        inline PacketType GetType() const { return type; }

        inline uint8_t GetChannelNumber() const { return channel_number; }

        inline uint32_t GetId() const { return id; }

        inline const std::vector<uint8_t>& GetData() const { return data; }

        inline std::chrono::system_clock::time_point GetTimestamp() const { return timestamp; }

        inline void SetTimestamp(std::chrono::system_clock::time_point new_timestamp) {
            using namespace std::chrono;
            timestamp = time_point_cast<microseconds>(new_timestamp);
        }

        inline void SetTimestampToNow() {
            using namespace std::chrono;
            timestamp = time_point_cast<microseconds>(system_clock::now());
        }
    };
} // namespace trudppp

#endif
