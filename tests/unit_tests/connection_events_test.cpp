#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/callbacks.hpp"
#include "trudppp/connection.hpp"

TEST(ConnectionEventsTest, InitAndDestroyEvents) {
    trudppp::Callbacks callbacks;

    std::vector<int> callback_calls;

    callbacks.connection_initialized = [&callback_calls](trudppp::Connection<std::function>&) { callback_calls.push_back(1); };

    callbacks.connection_destroyed = [&callback_calls](trudppp::Connection<std::function>&) { callback_calls.push_back(2); };

    {
        trudppp::Connection connection(callbacks);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 2}));
}
