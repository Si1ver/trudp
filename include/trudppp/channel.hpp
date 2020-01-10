#pragma once

#ifndef TRUDPPP_CHANNEL_HPP
#define TRUDPPP_CHANNEL_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <queue>
#include <vector>

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
    public:
        // TODO(new): add receive_timestamp.
        typedef void DataReceivedCallback(
            const std::vector<uint8_t>& received_data, bool is_reliable);
        typedef void PacketSendRequestedCallback(const Packet& packet_to_send);
        typedef void ChannelResetCallback();

        struct Settings {
            uint8_t channel_number;

            std::function<DataReceivedCallback> data_received_callback;
            std::function<PacketSendRequestedCallback> packet_send_requested_callback;
            std::function<ChannelResetCallback> channel_reset_callback;
        };

        struct Callbacks {
            std::function<DataReceivedCallback> data_received;
            std::function<PacketSendRequestedCallback> packet_send_requested;
            std::function<ChannelResetCallback> channel_reset;

            inline void EmitDataReceived(
                const std::vector<uint8_t>& received_data, bool is_reliable) const {
                if (data_received) {
                    data_received(received_data, is_reliable);
                }
            }

            inline void EmitSendPacketRequested(const Packet& packet_to_send) const {
                if (packet_send_requested) {
                    packet_send_requested(packet_to_send);
                }
            }

            inline void EmitChannelReset() const {
                if (channel_reset) {
                    channel_reset();
                }
            }
        };

    private:
        typedef std::queue<SendQueueItem> SendQueueType;
        typedef std::queue<void*> WriteQueueType;
        typedef std::map<uint32_t, ReceivedPacketItem> ReceivedPacketsType;

        const uint8_t channel_number;

        Callbacks callbacks;

        uint32_t next_send_id;
        uint32_t expected_receive_id;

        SendQueueType send_queue;
        WriteQueueType write_queue;

        ReceivedPacketsType received_packets;

        Channel(const Channel&) = delete;
        Channel& operator=(const Channel&) = delete;

        Packet CreateAckPacket(const Packet& received_packet) const;

        void Reset();

        inline uint32_t IncrementPacketId(uint32_t id) {
            ++id;

            // This check is ported from legacy code.
            // Packet with id = 0 is used to reset connection.
            if (id == 0) {
                ++id;
            }

            return id;
        }

    public:
        Channel(const Settings& settings)
            : channel_number(settings.channel_number), next_send_id(0),
              expected_receive_id(0) {
            callbacks.data_received = settings.data_received_callback;
            callbacks.packet_send_requested = settings.packet_send_requested_callback;
            callbacks.channel_reset = settings.channel_reset_callback;
        }

        // inline uint32_t GetCurrentSendId() const { return next_send_id; }

        // TODO: add receive_timestamp.
        void ProcessReceivedPacket(const Packet& received_packet);

        void SendData(const std::vector<uint8_t>& received_data);

        // TODO(new): Use trudp header for unreliable packets.
        void ProcessReceivedUnreliableData(const std::vector<uint8_t>& received_data) {
            callbacks.EmitDataReceived(received_data, false);
        }
    };
} // namespace trudppp

#endif
