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
        Packet data_packet(PacketType::Data, 0, 1, packet_data, timestamp);

        channel.ProcessReceivedPacket(data_packet);

        Packet ack_packet(PacketType::Ack, 0, 1, timestamp);
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
        Packet data_packet(PacketType::Ping, 0, 1, packet_data, timestamp);

        channel.ProcessReceivedPacket(data_packet);

        Packet ack_packet(PacketType::AckOnPing, 0, 1, packet_data, timestamp);
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
        Packet data_packet(PacketType::Reset, 0, 1, packet_data, timestamp);

        channel.ProcessReceivedPacket(data_packet);

        Packet ack_packet(PacketType::AckOnReset, 0, 1, timestamp);
        std::vector<uint8_t> serialized_packet = internal::SerializePacket(ack_packet);

        EXPECT_THAT(packet_received_calls, testing::IsEmpty());

        ASSERT_THAT(request_send_calls, testing::SizeIs(1));
        EXPECT_THAT(request_send_calls[0], testing::ElementsAreArray(serialized_packet));
    }
}
