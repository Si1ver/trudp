#include "trudppp/serialized_packet.hpp"

#include <cstdint>
#include <iterator>
#include <vector>

#include "trudppp/constants.hpp"
#include "trudppp/timestamp.hpp"

namespace trudppp::internal {
    uint32_t SerializeTimestamp(const Timestamp& timestamp) {
        int64_t time_since_epoch_us = timestamp.MicrosecondsSinceEpoch();

        // On serialization timestamp is truncated to 32 bits.
        return static_cast<uint32_t>(time_since_epoch_us);
    }

    Timestamp DeserializeTimestamp(uint32_t serialized_timestamp) {
        // Serialized trudppp timestamp is overflowing approximately every hour.
        // To restore original value we assume that packet was sent less than an hour ago.
        // Add ten minutes to compensate possible time difference between local and remove hosts.
        Timestamp timestamp = Timestamp::Now() + Duration::Minutes(10);

        int64_t reference_time_since_epoch_us = timestamp.MicrosecondsSinceEpoch();

        // On serialization timestamp is truncated to 32 bits.
        uint32_t reference_serialized_timestamp = static_cast<uint32_t>(reference_time_since_epoch_us);

        // Subtraction work correctly even if overflow occurred between two moments.
        int32_t difference_us = reference_serialized_timestamp - serialized_timestamp;

        return timestamp - Duration::Microseconds(difference_us);
    }

    std::vector<uint8_t> DeserializePacketData(const std::vector<uint8_t>& received_data) {
        // Packet data in serialized packet is located after header.
        auto data_begin = received_data.cbegin();
        std::advance(data_begin, kMinimumSerializedMessageSize);

        auto data_end = received_data.cend();

        return std::vector<uint8_t>(data_begin, data_end);
    }

    uint8_t CalculatePacketHeaderChecksum(const std::vector<uint8_t>& received_data) {
        // Header checksum starts at second byte of the header.
        auto header_begin = received_data.cbegin();
        std::advance(header_begin, 1);

        auto header_end = received_data.cbegin();
        std::advance(header_end, kMinimumSerializedMessageSize);

        return CalculateChecksum(header_begin, header_end);
    }

    bool CheckBufferIsValidPacket(const std::vector<uint8_t>& received_data) {
        size_t received_data_length = received_data.size();

        // Valid trudppp packet must at least contain packet header.
        if (received_data_length < kMinimumSerializedMessageSize) {
            return false;
        }

        const SerializedPacketHeader* packet_header =
            reinterpret_cast<const SerializedPacketHeader*>(received_data.data());

        // Protocol version must match exactly.
        if (packet_header->protocol_version != kTrudpppProtocolVersion) {
            return false;
        }

        // Packet payload length must be correctly specified.
        if (kMinimumSerializedMessageSize + packet_header->data_length != received_data_length) {
            return false;
        }

        // Packet type must have valid value.
        if (!SerializedPacketTypeIsValid(packet_header->packet_type)) {
            return false;
        }

        // Header checksum must be correct.
        uint8_t header_checksum = CalculatePacketHeaderChecksum(received_data);
        if (packet_header->header_checksum != header_checksum) {
            return false;
        }

        return true;
    }

    PacketInternal DeserializePacket(const std::vector<uint8_t>& received_data) {
        if (!CheckBufferIsValidPacket(received_data)) {
            return PacketInternal();
        }

        const SerializedPacketHeader* packet_header =
            reinterpret_cast<const SerializedPacketHeader*>(received_data.data());

        PacketType packet_type = DeserializePacketType(packet_header->packet_type);

        std::vector<uint8_t> packet_data = DeserializePacketData(received_data);

        Timestamp timestamp = DeserializeTimestamp(packet_header->timestamp);

        return PacketInternal(packet_type, packet_header->channel_number, packet_header->id,
            std::move(packet_data), timestamp);
    }

    std::vector<uint8_t> SerializePacket(const PacketInternal& packet) {
        const std::vector<uint8_t>& packet_data = packet.GetData();

        const size_t packet_data_length = packet_data.size();
        const size_t serialized_length = packet_data_length + kMinimumSerializedMessageSize;

        // Allocate size for all elements.
        std::vector<uint8_t> buffer(serialized_length);

        // Only resize vector to header size.
        buffer.resize(kMinimumSerializedMessageSize, 0);

        SerializedPacketHeader* packet_header =
            reinterpret_cast<SerializedPacketHeader*>(buffer.data());

        packet_header->protocol_version = kTrudpppProtocolVersion;
        packet_header->packet_type = SerializePacketType(packet.GetType());
        packet_header->channel_number = packet.GetChannelNumber();
        packet_header->data_length = packet_data_length;
        packet_header->id = packet.GetId();
        packet_header->timestamp = SerializeTimestamp(packet.GetTimestamp());

        packet_header->header_checksum = CalculatePacketHeaderChecksum(buffer);

        if (packet_data_length != 0) {
            buffer.insert(buffer.end(), packet_data.cbegin(), packet_data.cend());
        }

        return buffer;
    }

} // namespace trudppp::internal
