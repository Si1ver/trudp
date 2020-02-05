#include <cstdint>
#include <functional>
#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/service.hpp"
#include "trudppp/serialized_packet.hpp"
#include "trudppp/timestamp.hpp"

using namespace trudppp;

namespace {
    const std::vector<uint8_t> CreateTestPacket() {
        using namespace std::chrono;

        const PacketType packet_type = PacketType::Data;
        const uint8_t channel = 1;
        const uint32_t packet_id = 0;
        const std::vector<uint8_t> data = {};
        const Timestamp timestamp;

        PacketInternal test_packet(packet_type, channel, packet_id, data, timestamp);

        return internal::SerializePacket(test_packet);
    }

    class TestEndpoint {
        private:
            int endpoint_id;

        public:
            TestEndpoint(int endpoint_id) : endpoint_id(endpoint_id) {}

            bool operator==(const TestEndpoint& other) const {
                return endpoint_id == other.endpoint_id;
            }

            friend struct std::hash<TestEndpoint>;
    };
}

// Custom specialization of std::hash for TestEndpoint.
namespace std
{
    template<> struct hash<TestEndpoint>
    {
        size_t operator()(TestEndpoint const& endpoint) const noexcept
        {
            return std::hash<int>{}(endpoint.endpoint_id);
        }
    };
}

TEST(ServiceTest, CreateService) {
    Service<TestEndpoint>::Settings service_settings;

    std::vector<int> callback_calls;

    service_settings.accept_incoming_connections = true;
    service_settings.connection_initialized_callback = [&callback_calls](Connection&) { callback_calls.push_back(1); };
    service_settings.connection_destroyed_callback = [&callback_calls](Connection&) { callback_calls.push_back(2); };

    {
        Service<TestEndpoint> service(service_settings);

        EXPECT_THAT(callback_calls, testing::IsEmpty());
    }

    EXPECT_THAT(callback_calls, testing::IsEmpty());
}

TEST(ServiceTest, CreateServiceAndReceive) {
    Service<TestEndpoint>::Settings service_settings;

    std::vector<int> callback_calls;

    service_settings.accept_incoming_connections = true;
    service_settings.connection_initialized_callback = [&callback_calls](Connection&) { callback_calls.push_back(1); };
    service_settings.connection_destroyed_callback = [&callback_calls](Connection&) { callback_calls.push_back(2); };

    const std::vector<uint8_t> serialized_packet = CreateTestPacket();

    {
        Service<TestEndpoint> service(service_settings);

        EXPECT_THAT(callback_calls, testing::IsEmpty());

        service.ProcessReceivedData(1, serialized_packet);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1}));

        service.ProcessReceivedData(2, serialized_packet);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 2}));
}

TEST(ServiceTest, CreateServiceAndReject) {
    Service<TestEndpoint>::Settings service_settings;

    std::vector<int> callback_calls;

    service_settings.accept_incoming_connections = false;
    service_settings.connection_initialized_callback = [&callback_calls](Connection&) { callback_calls.push_back(1); };
    service_settings.connection_destroyed_callback = [&callback_calls](Connection&) { callback_calls.push_back(2); };

    const std::vector<uint8_t> serialized_packet = CreateTestPacket();

    {
        Service<TestEndpoint> service(service_settings);

        EXPECT_THAT(callback_calls, testing::IsEmpty());

        service.ProcessReceivedData(1, serialized_packet);

        EXPECT_THAT(callback_calls, testing::IsEmpty());

        service.ProcessReceivedData(2, serialized_packet);

        EXPECT_THAT(callback_calls, testing::IsEmpty());
    }

    EXPECT_THAT(callback_calls, testing::IsEmpty());
}