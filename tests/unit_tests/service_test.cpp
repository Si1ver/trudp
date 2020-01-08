#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/service.hpp"
#include "trudppp/serialized_packet.hpp"

using namespace trudppp;

const std::vector<uint8_t> CreateTestPacket() {
    using namespace std::chrono;

    const PacketType packet_type = PacketType::Data;
    const uint8_t channel = 1;
    const uint32_t packet_id = 0;
    const std::vector<uint8_t> data = {};
    const auto now_us = time_point_cast<microseconds>(system_clock::now());

    Packet test_packet(packet_type, channel, packet_id, data, now_us);

    return internal::SerializePacket(test_packet);
}

TEST(ServiceTest, CreateService) {
    ServiceSettings service_settings;

    std::vector<int> callback_calls;

    service_settings.accept_incoming_connections = true;
    service_settings.connection_initialized_callback = [&callback_calls](Connection<std::function>&) { callback_calls.push_back(1); };
    service_settings.connection_destroyed_callback = [&callback_calls](Connection<std::function>&) { callback_calls.push_back(2); };

    {
        Service<int> service(service_settings);

        EXPECT_THAT(callback_calls, testing::IsEmpty());
    }

    EXPECT_THAT(callback_calls, testing::IsEmpty());
}

TEST(ServiceTest, CreateServiceAndReceive) {
    ServiceSettings service_settings;

    std::vector<int> callback_calls;

    service_settings.accept_incoming_connections = true;
    service_settings.connection_initialized_callback = [&callback_calls](Connection<std::function>&) { callback_calls.push_back(1); };
    service_settings.connection_destroyed_callback = [&callback_calls](Connection<std::function>&) { callback_calls.push_back(2); };

    const std::vector<uint8_t> serialized_packet = CreateTestPacket();

    {
        Service<int> service(service_settings);

        EXPECT_THAT(callback_calls, testing::IsEmpty());

        service.ProcessReceivedData(1, serialized_packet);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1}));

        service.ProcessReceivedData(2, serialized_packet);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1}));
}

TEST(ServiceTest, CreateServiceAndReject) {
    ServiceSettings service_settings;

    std::vector<int> callback_calls;

    service_settings.accept_incoming_connections = false;
    service_settings.connection_initialized_callback = [&callback_calls](Connection<std::function>&) { callback_calls.push_back(1); };
    service_settings.connection_destroyed_callback = [&callback_calls](Connection<std::function>&) { callback_calls.push_back(2); };

    const std::vector<uint8_t> serialized_packet = CreateTestPacket();

    {
        Service<int> service(service_settings);

        EXPECT_THAT(callback_calls, testing::IsEmpty());

        service.ProcessReceivedData(1, serialized_packet);

        EXPECT_THAT(callback_calls, testing::IsEmpty());

        service.ProcessReceivedData(2, serialized_packet);

        EXPECT_THAT(callback_calls, testing::IsEmpty());
    }

    EXPECT_THAT(callback_calls, testing::IsEmpty());
}