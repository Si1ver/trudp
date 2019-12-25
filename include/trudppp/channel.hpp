#pragma once

#ifndef TRUDPPP_CHANNEL_HPP
#define TRUDPPP_CHANNEL_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <queue>

#include <trudppp/callbacks.hpp>
#include <trudppp/packet.hpp>

namespace trudppp {
    struct SendQueueItem {
    private:
        SendQueueItem(const SendQueueItem&) = delete;

        SendQueueItem& operator=(const SendQueueItem&) = delete;

    public:
        std::chrono::system_clock::time_point expected_time;
        uint32_t retry_count;
        std::chrono::system_clock::time_point next_retry_time;
        Packet packet;

        SendQueueItem(SendQueueItem&& other) noexcept
            : expected_time(other.expected_time), retry_count(other.retry_count),
              next_retry_time(other.next_retry_time), packet(std::move(packet)) {}
    };

    template <template <class> class TCallback = std::function>
    class Channel {
    private:
        uint32_t next_send_id;
        std::queue<SendQueueItem> send_queue;
        std::queue<void*> write_queue;
        std::queue<void*> receive_queue;

        const Callbacks<TCallback>& callbacks;

        Channel(const Channel&) = delete;

        Channel& operator=(const Channel&) = delete;

    public:
        Channel(const Callbacks<TCallback>& callbacks) : callbacks(callbacks) {}

        inline uint32_t GetCurrentSendId() { return next_send_id; }

        inline uint32_t GetNextSendId() {
            uint32_t current_send_id = next_send_id;

            ++next_send_id;

            // Packet with id = 0 is used to reset connection.
            if (next_send_id == 0) {
                ++next_send_id;
            }
        }
    };
} // namespace trudppp

#endif
