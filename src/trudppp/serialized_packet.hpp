// This is internal header file that should not be included in any public headers.

#pragma once

#ifndef TRUDPPP_SERIALIZED_PACKET_HPP
#define TRUDPPP_SERIALIZED_PACKET_HPP

#include <chrono>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "trudppp/constants.hpp"
#include "trudppp/packet.hpp"

namespace trudppp::internal {
#pragma pack(push)
#pragma pack(1)
    /// Serialized trudppp message header.
    struct SerializedPacketHeader {
        uint8_t header_checksum;      ///< Checksum of the packet header.
        uint8_t protocol_version : 4; ///< Protocol version.
        uint8_t packet_type : 4;      ///< Type of the packet.
        // uint16_t type of field channel_number is needed to ensure correct data packing.
        uint16_t channel_number : 4; ///< Channel number.
        uint16_t data_length : 12;   ///< The length of payload data.
        uint32_t id;                 ///< Packet identifier.
        uint32_t timestamp;          /// The moment of time associated with a packet.
    };
#pragma pack(pop)

    // This check ensure correct packing for serialized header at compile time.
    static_assert(sizeof(SerializedPacketHeader) == kMinimumSerializedMessageSize);

    uint8_t constexpr SerializePacketType(PacketType packet_type) {
        return static_cast<uint8_t>(packet_type);
    }

    PacketType constexpr DeserializePacketType(uint8_t serialized_packet_type) {
        // Expand to underlying type of enumeration.
        std::underlying_type_t<PacketType> serialized_value = serialized_packet_type;

        switch (serialized_value) {
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Data):
                return PacketType::Data;

            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Ack):
                return PacketType::Ack;

            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Reset):
                return PacketType::Reset;

            case static_cast<std::underlying_type_t<PacketType>>(PacketType::AckOnReset):
                return PacketType::AckOnReset;

            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Ping):
                return PacketType::Ping;

            case static_cast<std::underlying_type_t<PacketType>>(PacketType::AckOnPing):
                return PacketType::AckOnPing;

            default:
                throw std::invalid_argument("Unknown packet type value.");
        }
    }

    uint8_t constexpr SerializedPacketTypeIsValid(uint8_t serialized_packet_type) {
        // Expand to underlying type of enumeration.
        std::underlying_type_t<PacketType> serialized_value = serialized_packet_type;

        switch (serialized_value) {
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Data):
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Ack):
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Reset):
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::AckOnReset):
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::Ping):
            case static_cast<std::underlying_type_t<PacketType>>(PacketType::AckOnPing):
                return true;

            default:
                return false;
        }
    }

    template <class Iterator>
    // This constraint allows iteration only on uint8_t containers.
    typename std::enable_if<
        std::is_same<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value,
        uint8_t>::type
    CalculateChecksum(Iterator it, Iterator end) {
        uint8_t result = 0;

        while (it != end) {
            result += *it;
            ++it;
        }

        return result;
    }

    uint32_t SerializeTimestamp(std::chrono::system_clock::time_point timestamp);

    std::chrono::system_clock::time_point DeserializeTimestamp(uint32_t timestamp);

    std::vector<uint8_t> DeserializePacketData(const std::vector<uint8_t>& received_data);

    uint8_t CalculatePacketHeaderChecksum(const std::vector<uint8_t>& received_data);

    bool CheckBufferIsValidPacket(const std::vector<uint8_t>& received_data);

    Packet DeserializePacket(const std::vector<uint8_t>& received_data);

    std::vector<uint8_t> SerializePacket(const Packet& packet);
} // namespace trudppp::internal
#endif
