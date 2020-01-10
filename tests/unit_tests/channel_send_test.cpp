#include <chrono>
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/callbacks.hpp"
#include "trudppp/channel.hpp"
#include "trudppp/connection.hpp"
#include "trudppp/packet.hpp"

#include "trudppp/serialized_packet.hpp"

using namespace trudppp;

namespace {
    using namespace std::chrono;

    system_clock::time_point GetTimeNowUs() {
        return time_point_cast<microseconds>(system_clock::now());
    }
}

TEST(ChannelSendTest, SendData) {
    Callbacks callbacks;

    std::vector<Packet> packet_received_calls;
    std::vector<std::vector<uint8_t>> request_send_calls;

    callbacks.packet_received = [&packet_received_calls](Connection&, Channel&, const Packet& received_packet) { packet_received_calls.emplace_back(received_packet); };
    callbacks.send_data_request = [&request_send_calls](Connection&, Channel&, const std::vector<uint8_t>& received_data) { request_send_calls.emplace_back(received_data); };

    {
        // In this test Connection object is needed only to pass it as a callback argument.
        Connection connection(callbacks);
        Channel channel(callbacks, connection);

        EXPECT_THAT(request_send_calls, testing::IsEmpty());
        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        auto timestamp = GetTimeNowUs();

        std::vector<uint8_t> packet_data = {1, 2, 3, 4, 5};
        Packet data_packet(PacketType::Data, 0, 0, packet_data, timestamp);

        channel.ProcessReceivedPacket(data_packet);

        Packet ack_packet(PacketType::Ack, 0, 0, timestamp);
        std::vector<uint8_t> serialized_packet = internal::SerializePacket(ack_packet);

        ASSERT_THAT(packet_received_calls, testing::SizeIs(1));
        EXPECT_THAT(packet_received_calls[0].GetData(), testing::ElementsAreArray(packet_data));

        ASSERT_THAT(request_send_calls, testing::SizeIs(1));
        EXPECT_THAT(request_send_calls[0], testing::ElementsAreArray(serialized_packet));
    }
}

TEST(ChannelSendTest, SendPing) {
    Callbacks callbacks;

    std::vector<Packet> packet_received_calls;
    std::vector<std::vector<uint8_t>> request_send_calls;

    callbacks.packet_received = [&packet_received_calls](Connection&, Channel&, const Packet& received_packet) { packet_received_calls.emplace_back(received_packet); };
    callbacks.send_data_request = [&request_send_calls](Connection&, Channel&, const std::vector<uint8_t>& received_data) { request_send_calls.emplace_back(received_data); };

    {
        // In this test Connection object is needed only to pass it as a callback argument.
        Connection connection(callbacks);
        Channel channel(callbacks, connection);

        EXPECT_THAT(request_send_calls, testing::IsEmpty());
        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        auto timestamp = GetTimeNowUs();

        std::vector<uint8_t> packet_data = {1, 2, 3, 4, 5};
        Packet data_packet(PacketType::Ping, 0, 0, packet_data, timestamp);

        channel.ProcessReceivedPacket(data_packet);

        Packet ack_packet(PacketType::AckOnPing, 0, 0, packet_data, timestamp);
        std::vector<uint8_t> serialized_packet = internal::SerializePacket(ack_packet);

        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        ASSERT_THAT(request_send_calls, testing::SizeIs(1));
        EXPECT_THAT(request_send_calls[0], testing::ElementsAreArray(serialized_packet));
    }
}

TEST(ChannelSendTest, SendReset) {
    Callbacks callbacks;

    std::vector<Packet> packet_received_calls;
    std::vector<std::vector<uint8_t>> request_send_calls;

    callbacks.packet_received = [&packet_received_calls](Connection&, Channel&, const Packet& received_packet) { packet_received_calls.emplace_back(received_packet); };
    callbacks.send_data_request = [&request_send_calls](Connection&, Channel&, const std::vector<uint8_t>& received_data) { request_send_calls.emplace_back(received_data); };

    {
        // In this test Connection object is needed only to pass it as a callback argument.
        Connection connection(callbacks);
        Channel channel(callbacks, connection);

        EXPECT_THAT(request_send_calls, testing::IsEmpty());
        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        auto timestamp = GetTimeNowUs();

        std::vector<uint8_t> packet_data = {1, 2, 3, 4, 5};
        Packet data_packet(PacketType::Reset, 0, 0, packet_data, timestamp);

        channel.ProcessReceivedPacket(data_packet);

        Packet ack_packet(PacketType::AckOnReset, 0, 0, timestamp);
        std::vector<uint8_t> serialized_packet = internal::SerializePacket(ack_packet);

        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        ASSERT_THAT(request_send_calls, testing::SizeIs(1));
        EXPECT_THAT(request_send_calls[0], testing::ElementsAreArray(serialized_packet));
    }
}

TEST(ChannelSendTest, ReceiveOutOfOrder) {
    Callbacks callbacks;

    std::vector<Packet> packet_received_calls;
    std::vector<std::vector<uint8_t>> request_send_calls;

    callbacks.packet_received = [&packet_received_calls](Connection&, Channel&, const Packet& received_packet) { packet_received_calls.emplace_back(received_packet); };
    callbacks.send_data_request = [&request_send_calls](Connection&, Channel&, const std::vector<uint8_t>& received_data) { request_send_calls.emplace_back(received_data); };

    {
        // In this test Connection object is needed only to pass it as a callback argument.
        Connection connection(callbacks);
        Channel channel(callbacks, connection);

        EXPECT_THAT(request_send_calls, testing::IsEmpty());
        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        auto timestamp2 = GetTimeNowUs();
        auto timestamp1 = timestamp2 - milliseconds(10);
        auto timestamp0 = timestamp1 - milliseconds(20);

        const uint8_t channel_number = 0;

        const std::vector<uint8_t> packet1_data = {1, 2, 3, 4, 5};
        const std::vector<uint8_t> packet2_data = {5, 4, 3, 2, 1, 0};

        const Packet data0_packet(PacketType::Data, channel_number, 0, timestamp0);
        const Packet data1_packet(PacketType::Data, channel_number, 1, packet1_data, timestamp1);
        const Packet data2_packet(PacketType::Data, channel_number, 2, packet2_data, timestamp2);

        const Packet ack0_packet(PacketType::Ack, channel_number, 0, timestamp0);
        const Packet ack1_packet(PacketType::Ack, channel_number, 1, timestamp1);
        const Packet ack2_packet(PacketType::Ack, channel_number, 2, timestamp2);

        const std::vector<uint8_t> serialized_ack0_packet = internal::SerializePacket(ack0_packet);
        const std::vector<uint8_t> serialized_ack1_packet = internal::SerializePacket(ack1_packet);
        const std::vector<uint8_t> serialized_ack2_packet = internal::SerializePacket(ack2_packet);

        channel.ProcessReceivedPacket(data0_packet);

        ASSERT_THAT(packet_received_calls, testing::SizeIs(1));
        EXPECT_THAT(packet_received_calls[0].GetData(), testing::IsEmpty());

        ASSERT_THAT(request_send_calls, testing::SizeIs(1));
        EXPECT_THAT(request_send_calls[0], testing::ElementsAreArray(serialized_ack0_packet));

        channel.ProcessReceivedPacket(data2_packet);

        ASSERT_THAT(packet_received_calls, testing::SizeIs(1));

        ASSERT_THAT(request_send_calls, testing::SizeIs(2));
        EXPECT_THAT(request_send_calls[1], testing::ElementsAreArray(serialized_ack2_packet));

        channel.ProcessReceivedPacket(data1_packet);

        ASSERT_THAT(packet_received_calls, testing::SizeIs(3));
        EXPECT_THAT(packet_received_calls[1].GetData(), testing::ElementsAreArray(packet1_data));
        EXPECT_THAT(packet_received_calls[2].GetData(), testing::ElementsAreArray(packet2_data));

        ASSERT_THAT(request_send_calls, testing::SizeIs(3));
        EXPECT_THAT(request_send_calls[2], testing::ElementsAreArray(serialized_ack1_packet));
    }
}
