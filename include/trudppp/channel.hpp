#pragma once

#ifndef TRUDPPP_CHANNEL_HPP
#define TRUDPPP_CHANNEL_HPP

#include <chrono>
#include <cstdint>
#include <map>
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
              next_retry_time(other.next_retry_time), packet(std::move(other.packet)) {}
    };

    struct ReceivedPacketItem {
    private:
        ReceivedPacketItem(const ReceivedPacketItem&) = delete;

        ReceivedPacketItem& operator=(const ReceivedPacketItem&) = delete;

    public:
        std::chrono::system_clock::time_point receive_timestamp;
        Packet packet;

        ReceivedPacketItem(
            std::chrono::system_clock::time_point receive_timestamp, const Packet& packet)
            : receive_timestamp(receive_timestamp), packet(packet) {}

        ReceivedPacketItem(
            std::chrono::system_clock::time_point receive_timestamp, Packet&& packet) noexcept
            : receive_timestamp(receive_timestamp), packet(std::move(packet)) {}

        ReceivedPacketItem(ReceivedPacketItem&& other) noexcept
            : receive_timestamp(other.receive_timestamp), packet(std::move(other.packet)) {}
    };

    class Channel {
    private:
        uint32_t next_send_id;
        uint32_t expected_receive_id;
        std::queue<SendQueueItem> send_queue;
        std::queue<void*> write_queue;
        std::map<uint32_t, ReceivedPacketItem> received_packets;

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

        inline void EmitSendDataRequestCallback(const std::vector<uint8_t>& data_to_send) {
            if (callbacks.send_data_request) {
                callbacks.send_data_request(connection, *this, data_to_send);
            }
        }

        void RequestSendPacket(const Packet& packet_to_send);

        Packet CreateAckPacket(const Packet& received_packet) const;

    public:
        Channel(const Callbacks& callbacks, Connection& connection)
            : callbacks(callbacks), connection(connection), next_send_id(0),
              expected_receive_id(0){}

        inline uint32_t GetCurrentSendId() const { return next_send_id; }

        inline uint32_t GetNextSendId() {
            uint32_t current_send_id = next_send_id;

            ++next_send_id;

            // Packet with id = 0 is used to reset connection.
            if (next_send_id == 0) {
                ++next_send_id;
            }
        }

        void ProcessReceivedPacket(const Packet& received_packet);

        void ProcessReceivedUnreliableData(const std::vector<uint8_t>& received_data) {
            EmitUnreliableDataReceivedCallback(received_data);
        }
    };
} // namespace trudppp

#endif
