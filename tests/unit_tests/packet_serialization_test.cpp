#include <cstdint>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/constants.hpp"
#include "trudppp/serialized_packet.hpp"
#include "trudppp/timestamp.hpp"

using namespace trudppp;

MATCHER_P(TimestampIsEqual, timestamp, "") { return arg == timestamp; }

TEST(SerializedPacketTest, TimestampRoundtrip) {
    Timestamp now;

    uint32_t serialized_now = internal::SerializeTimestamp(now);

    Timestamp deserialized_timestamp = internal::DeserializeTimestamp(serialized_now);

    EXPECT_THAT(deserialized_timestamp, TimestampIsEqual(now));
}

TEST(SerializedPacketTest, TimestampConsistense) {
    Timestamp now;

    uint32_t serialized_now = internal::SerializeTimestamp(now);

    const int64_t advancement = 1000;

    Timestamp advanced_now(now);
    advanced_now.ShiftMicroseconds(advancement);

    uint32_t serialized_advanced_now = internal::SerializeTimestamp(advanced_now);

    EXPECT_EQ(serialized_advanced_now - serialized_now, advancement);
}

TEST(SerializedPacketTest, DeserializeData) {
    const std::vector<uint8_t> data = {1, 2, 0, 7, 255};

    std::vector<uint8_t> buffer(kMinimumSerializedMessageSize + data.size());

    // Add space that normally occupied by packet header.
    buffer.resize(kMinimumSerializedMessageSize, 0);

    buffer.insert(buffer.end(), data.cbegin(), data.cend());

    const std::vector<uint8_t> deserialized_data = internal::DeserializePacketData(buffer);

    EXPECT_THAT(deserialized_data, testing::ElementsAreArray(data));
}

TEST(SerializedPacketTest, DeserializeEmptyData) {
    std::vector<uint8_t> buffer(kMinimumSerializedMessageSize);
    buffer.resize(kMinimumSerializedMessageSize, 0);

    const std::vector<uint8_t> deserialized_data = internal::DeserializePacketData(buffer);

    EXPECT_EQ(deserialized_data.size(), 0);
}

TEST(SerializedPacketTest, PacketRoundtrip) {
    const PacketType packet_type = PacketType::Ping;
    const uint8_t channel = 1;
    const SequenceId packet_id = 10;
    const std::vector<uint8_t> data = {1, 2, 0, 7, 255};
    const Timestamp timestamp;

    PacketInternal original_packet(packet_type, channel, packet_id, data, timestamp);

    const std::vector<uint8_t> serialized_packet = internal::SerializePacket(original_packet);

    bool buffer_is_valid_packet = internal::CheckBufferIsValidPacket(serialized_packet);

    ASSERT_TRUE(buffer_is_valid_packet);

    PacketInternal deserialized_packet = internal::DeserializePacket(serialized_packet);

    EXPECT_EQ(deserialized_packet.GetType(), packet_type);
    EXPECT_EQ(deserialized_packet.GetChannelNumber(), channel);
    EXPECT_EQ(deserialized_packet.GetId(), packet_id);
    EXPECT_THAT(deserialized_packet.GetData(), testing::ElementsAreArray(data));
    EXPECT_THAT(deserialized_packet.GetTimestamp(), TimestampIsEqual(timestamp));
}
