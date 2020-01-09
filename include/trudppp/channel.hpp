#pragma once

#ifndef TRUDPPP_CHANNEL_HPP
#define TRUDPPP_CHANNEL_HPP

#include <chrono>
#include <cstdint>
#include <queue>
#include <vector>

#include <trudppp/callbacks.hpp>
#include <trudppp/packet.hpp>

namespace trudppp {
    class Connection;

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

    class Channel {
    private:
        uint32_t next_send_id;
        std::queue<SendQueueItem> send_queue;
        std::queue<void*> write_queue;
        std::queue<void*> receive_queue;

        const Callbacks& callbacks;

        Connection& connection;

        Channel(const Channel&) = delete;

        Channel& operator=(const Channel&) = delete;

        inline void EmitPacketReceivedCallback(const Packet& received_packet) {
            if (callbacks.packet_received) {
                callbacks.packet_received(connection, *this, received_packet);
            }
        }

        inline void EmitUnreliableDataReceivedCallback(const std::vector<uint8_t>& received_data) {
            if (callbacks.unreliable_data_received) {
                callbacks.unreliable_data_received(connection, *this, received_data);
            }
        }

    public:
        Channel(const Callbacks& callbacks, Connection& connection)
            : callbacks(callbacks), connection(connection), next_send_id(0) {}

        inline uint32_t GetCurrentSendId() { return next_send_id; }

        inline uint32_t GetNextSendId() {
            uint32_t current_send_id = next_send_id;

            ++next_send_id;

            // Packet with id = 0 is used to reset connection.
            if (next_send_id == 0) {
                ++next_send_id;
            }
        }

        void ProcessReceivedPacket(const Packet& received_packet) {}
        void ProcessReceivedUnreliableData(const std::vector<uint8_t>& received_data) {}
    };
} // namespace trudppp

#endif
