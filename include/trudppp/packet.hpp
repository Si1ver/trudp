#pragma once

#ifndef TRUDPPP_PACKET_HPP
#define TRUDPPP_PACKET_HPP

#include <cstdint>
#include <vector>

#include "trudppp/timestamp.hpp"

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
    class PacketInternal {
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
        Timestamp timestamp;

        PacketInternal& operator=(const PacketInternal&) = delete;

    public:
        PacketInternal() : type(PacketType::Data), channel_number(0), id(0) {}

        PacketInternal(PacketType type, uint8_t channel_number, uint32_t id, const Timestamp& timestamp)
            : type(type), channel_number(channel_number), id(id), timestamp(timestamp) {}

        PacketInternal(PacketType type, uint8_t channel_number, uint32_t id,
            const std::vector<uint8_t>& data, const Timestamp& timestamp)
            : type(type), channel_number(channel_number), id(id), data(data), timestamp(timestamp) {
        }

        PacketInternal(PacketType type, uint8_t channel_number, uint32_t id, std::vector<uint8_t>&& data,
            const Timestamp& timestamp)
            : type(type), channel_number(channel_number), id(id), data(std::move(data)),
              timestamp(timestamp) {}

        PacketInternal(const PacketInternal& other)
            : type(other.type), channel_number(other.channel_number), id(other.id),
              data(other.data), timestamp(other.timestamp) {}

        PacketInternal(PacketInternal&& other) noexcept
            : type(other.type), channel_number(other.channel_number), id(other.id),
              data(std::move(other.data)), timestamp(other.timestamp) {}

        inline PacketType GetType() const { return type; }

        inline uint8_t GetChannelNumber() const { return channel_number; }

        inline uint32_t GetId() const { return id; }

        inline const std::vector<uint8_t>& GetData() const { return data; }

        inline const Timestamp& GetTimestamp() const { return timestamp; }

        inline void SetTimestamp(const Timestamp& new_timestamp) { timestamp = new_timestamp; }

        inline void SetTimestampToNow() { timestamp.SetToNow(); }

        PacketInternal& operator=(PacketInternal&& packet) = default;
    };

    //Trudppp owning packet wrapper for client side usage
    class Packet
    {
        friend class Connection;
    public:
        explicit Packet(PacketInternal&& packet_) : packet(std::move(packet_)) {}
        ~Packet() = default;
        Packet(const Packet&) = delete;
        Packet(Packet&&) = default;

        Packet& operator=(const Packet&) = delete;
        Packet& operator=(Packet&&) = default;

        std::vector<uint8_t> Serialize() const;
    private:
        PacketInternal packet;
    };
} // namespace trudppp

#endif
