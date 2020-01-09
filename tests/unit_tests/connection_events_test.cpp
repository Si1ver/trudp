#include <vector>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "trudppp/callbacks.hpp"
#include "trudppp/connection.hpp"

using namespace trudppp;

TEST(ConnectionEventsTest, InitAndDestroyEvents) {
    Callbacks callbacks;

    std::vector<int> callback_calls;

    callbacks.connection_initialized = [&callback_calls](Connection&) { callback_calls.push_back(1); };

    callbacks.connection_destroyed = [&callback_calls](Connection&) { callback_calls.push_back(2); };

    {
        Connection connection_one(callbacks);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1}));

        {
            Connection connection_two(callbacks);

            EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1}));
        }

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2}));

        {
            Connection connection_three(callbacks);

            EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1}));
        }

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2, 2}));

    {
        Connection connection_four(callbacks);

        EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2, 2, 1}));
    }

    EXPECT_THAT(callback_calls, testing::ElementsAreArray({1, 1, 2, 1, 2, 2, 1, 2}));
}

TEST(ConnectionEventsTest, EmptyCallbacks) {
    Callbacks callbacks;

    std::vector<int> callback_calls;

    Connection connection_four(callbacks);
}
