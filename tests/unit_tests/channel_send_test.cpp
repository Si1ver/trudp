#include <algorithm>
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/channel.hpp"
#include "trudppp/packet.hpp"
#include "trudppp/timestamp.hpp"

using namespace trudppp;

namespace {
    const uint8_t kChannelNumber = 0;

    struct ChannelTestHelper {
        std::vector<std::vector<uint8_t>> data_received_calls;
        std::vector<Packet> packet_send_requested_calls;

        Channel::Settings settings;

        ChannelTestHelper() {
            settings.channel_number = kChannelNumber;
            settings.data_received_callback = [this](
                                                    const std::vector<uint8_t>& received_data,
                                                    bool is_reliable) {
                data_received_calls.emplace_back(received_data);
            };

            settings.packet_send_requested_callback = [this](const Packet& packet_to_send) {
                packet_send_requested_calls.emplace_back(packet_to_send);
            };
        }
    };

    bool DataIsEqual(const std::vector<uint8_t>& left, const std::vector<uint8_t>& right) {
        return left.size() == right.size() && std::equal(left.cbegin(), left.cend(), right.cbegin());
    }
}

MATCHER_P(PacketIsEqual, packet, "") { \
    return arg.GetType() == packet.GetType() && \
        arg.GetChannelNumber() == packet.GetChannelNumber() && \
        arg.GetId() == packet.GetId() && \
        DataIsEqual(packet.GetData(), packet.GetData()) && \
        arg.GetTimestamp() == packet.GetTimestamp(); \
    }

TEST(ChannelSendTest, SendData) {
    ChannelTestHelper test_helper;

    Timestamp timestamp;
    const std::vector<uint8_t> packet_data = {1, 2, 3, 4, 5};

    const Packet data_packet(PacketType::Data, kChannelNumber, 0, packet_data, timestamp);
    const Packet ack_packet(PacketType::Ack, kChannelNumber, 0, packet_data, timestamp);

    {
        Channel channel(test_helper.settings);

        EXPECT_THAT(test_helper.data_received_calls, testing::IsEmpty());
        EXPECT_THAT(test_helper.packet_send_requested_calls, testing::IsEmpty());

        channel.ProcessReceivedPacket(data_packet);

        ASSERT_THAT(test_helper.data_received_calls, testing::SizeIs(1));
        EXPECT_THAT(test_helper.data_received_calls[0], testing::ElementsAreArray(packet_data));

        ASSERT_THAT(test_helper.packet_send_requested_calls, testing::SizeIs(1));
        EXPECT_THAT(test_helper.packet_send_requested_calls[0], PacketIsEqual(ack_packet));
    }
}

TEST(ChannelSendTest, SendPing) {
    ChannelTestHelper test_helper;

    Timestamp timestamp;
    const std::vector<uint8_t> packet_data = {1, 2, 3, 4, 5};

    const Packet ping_packet(PacketType::Ping, kChannelNumber, 0, packet_data, timestamp);
    const Packet ack_packet(PacketType::AckOnPing, kChannelNumber, 0, timestamp);

    {
        Channel channel(test_helper.settings);

        EXPECT_THAT(test_helper.data_received_calls, testing::IsEmpty());
        EXPECT_THAT(test_helper.packet_send_requested_calls, testing::IsEmpty());

        channel.ProcessReceivedPacket(ping_packet);

        EXPECT_THAT(test_helper.data_received_calls, testing::IsEmpty());

        ASSERT_THAT(test_helper.packet_send_requested_calls, testing::SizeIs(1));
        EXPECT_THAT(test_helper.packet_send_requested_calls[0], PacketIsEqual(ack_packet));
    }
}

TEST(ChannelSendTest, SendReset) {
    ChannelTestHelper test_helper;

    Timestamp timestamp;
    const std::vector<uint8_t> packet_data = {1, 2, 3, 4, 5};

    const Packet reset_packet(PacketType::Reset, kChannelNumber, 0, packet_data, timestamp);
    const Packet ack_packet(PacketType::AckOnReset, kChannelNumber, 0, timestamp);

    {
        Channel channel(test_helper.settings);

        EXPECT_THAT(test_helper.data_received_calls, testing::IsEmpty());
        EXPECT_THAT(test_helper.packet_send_requested_calls, testing::IsEmpty());

        channel.ProcessReceivedPacket(reset_packet);

        EXPECT_THAT(test_helper.data_received_calls, testing::IsEmpty());

        ASSERT_THAT(test_helper.packet_send_requested_calls, testing::SizeIs(1));
        EXPECT_THAT(test_helper.packet_send_requested_calls[0], PacketIsEqual(ack_packet));
    }
}

TEST(ChannelSendTest, ReceiveOutOfOrder) {
    ChannelTestHelper test_helper;

    Timestamp timestamp2;
    Timestamp timestamp1(timestamp2);
    timestamp1.ShiftMicroseconds(-10);
    Timestamp timestamp0(timestamp1);
    timestamp0.ShiftMicroseconds(-20);

    const std::vector<uint8_t> packet1_data = {1, 2, 3, 4, 5};
    const std::vector<uint8_t> packet2_data = {5, 4, 3, 2, 1, 0};

    const Packet data0_packet(PacketType::Data, kChannelNumber, 0, timestamp0);
    const Packet data1_packet(PacketType::Data, kChannelNumber, 1, packet1_data, timestamp1);
    const Packet data2_packet(PacketType::Data, kChannelNumber, 2, packet2_data, timestamp2);

    const Packet ack0_packet(PacketType::Ack, kChannelNumber, 0, timestamp0);
    const Packet ack1_packet(PacketType::Ack, kChannelNumber, 1, timestamp1);
    const Packet ack2_packet(PacketType::Ack, kChannelNumber, 2, timestamp2);

    {
        Channel channel(test_helper.settings);

        EXPECT_THAT(test_helper.data_received_calls, testing::IsEmpty());
        EXPECT_THAT(test_helper.packet_send_requested_calls, testing::IsEmpty());

        channel.ProcessReceivedPacket(data0_packet);

        ASSERT_THAT(test_helper.data_received_calls, testing::SizeIs(1));
        EXPECT_THAT(test_helper.data_received_calls[0], testing::IsEmpty());

        ASSERT_THAT(test_helper.packet_send_requested_calls, testing::SizeIs(1));
        EXPECT_THAT(test_helper.packet_send_requested_calls[0], PacketIsEqual(ack0_packet));

        channel.ProcessReceivedPacket(data2_packet);

        ASSERT_THAT(test_helper.data_received_calls, testing::SizeIs(1));

        ASSERT_THAT(test_helper.packet_send_requested_calls, testing::SizeIs(2));
        EXPECT_THAT(test_helper.packet_send_requested_calls[1], PacketIsEqual(ack2_packet));

        channel.ProcessReceivedPacket(data1_packet);

        ASSERT_THAT(test_helper.data_received_calls, testing::SizeIs(3));
        EXPECT_THAT(test_helper.data_received_calls[1], testing::ElementsAreArray(packet1_data));
        EXPECT_THAT(test_helper.data_received_calls[2], testing::ElementsAreArray(packet2_data));

        ASSERT_THAT(test_helper.packet_send_requested_calls, testing::SizeIs(3));
        EXPECT_THAT(test_helper.packet_send_requested_calls[2], PacketIsEqual(ack1_packet));
    }
}
