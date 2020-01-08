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
        trudppp::Connection connection_one(callbacks);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1}));

        {
            trudppp::Connection connection_two(callbacks);

            EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1}));
        }

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2}));

        {
            trudppp::Connection connection_three(callbacks);

            EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1}));
        }

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2, 2}));

    {
        trudppp::Connection connection_four(callbacks);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2, 2, 1}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2, 2, 1, 2}));
}

TEST(ConnectionEventsTest, EmptyCallbacks) {
    trudppp::Callbacks callbacks;

    std::vector<int> callback_calls;

    trudppp::Connection connection_four(callbacks);
}
