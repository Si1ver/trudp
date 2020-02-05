#pragma once

#ifndef TRUDPPP_CHANNEL_HPP
#define TRUDPPP_CHANNEL_HPP

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <optional>

#include <trudppp/callbacks.hpp>
#include <trudppp/packet.hpp>
#include <trudppp/timestamp.hpp>
#include <trudppp/constants.hpp>

namespace trudppp {
    struct SentPacketItem {
    private:
        SentPacketItem(const SentPacketItem&) = delete;
        SentPacketItem& operator=(const SentPacketItem&) = delete;

    public:
        Timestamp expected_time;
        uint32_t retry_count = 0;
        PacketInternal packet;

        SentPacketItem(SentPacketItem&& other) noexcept
            : expected_time(other.expected_time), retry_count(other.retry_count),
              packet(std::move(other.packet)) {}

        SentPacketItem(PacketInternal&& packet_, const Timestamp& expected_time_)
            : expected_time(expected_time_), packet(std::move(packet_)) {}

        SentPacketItem(const PacketInternal& packet_, const Timestamp& expected_time_)
            : expected_time(expected_time_), packet(packet_) {}
    };

    struct ReceivedPacketItem {
    private:
        ReceivedPacketItem(const ReceivedPacketItem&) = delete;
        ReceivedPacketItem& operator=(const ReceivedPacketItem&) = delete;

    public:
        Timestamp receive_timestamp;
        PacketInternal packet;

        ReceivedPacketItem(
            const Timestamp& receive_timestamp, const PacketInternal& packet)
            : receive_timestamp(receive_timestamp), packet(packet) {}

        ReceivedPacketItem(const Timestamp& receive_timestamp, PacketInternal&& packet) noexcept
            : receive_timestamp(receive_timestamp), packet(std::move(packet)) {}

        ReceivedPacketItem(ReceivedPacketItem&& other) noexcept
            : receive_timestamp(other.receive_timestamp), packet(std::move(other.packet)) {}
    };

    struct ScheduledPacketItem {
    private:

        ScheduledPacketItem(const ScheduledPacketItem&) = delete;
        ScheduledPacketItem& operator=(const ScheduledPacketItem&) = delete;
    public:
        PacketInternal packet;

        ~ScheduledPacketItem() = default;
        ScheduledPacketItem(ScheduledPacketItem&&) = default;
        ScheduledPacketItem& operator=(ScheduledPacketItem&&) = default;

        explicit ScheduledPacketItem(PacketInternal&& packet_) noexcept : packet(std::move(packet_)) {}
    };

    class Channel {
    public:
        // TODO(new): add receive_timestamp.
        typedef void DataReceivedCallback(
            const std::vector<uint8_t>& received_data, bool is_reliable);
        typedef void PacketSendRequestedCallback(PacketInternal&& packet_to_send);
        typedef void ChannelResetCallback();
        typedef void AckReceivedCallback(uint32_t packet_id);

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
            std::function<AckReceivedCallback> ack_received;

            inline void EmitDataReceived(
                const std::vector<uint8_t>& received_data, bool is_reliable) const {
                if (data_received) {
                    data_received(received_data, is_reliable);
                }
            }

            inline void EmitSendPacketRequested(PacketInternal&& packet_to_send) const {
                if (packet_send_requested) {
                    packet_send_requested(std::move(packet_to_send));
                }
            }

            inline void EmitChannelReset() const {
                if (channel_reset) {
                    channel_reset();
                }
            }

            inline void EmitAckReceived(uint32_t packet_id) const {
                if (ack_received) {
                    ack_received(packet_id);
                }
            }
        };

    private:
        typedef std::list<SentPacketItem> SentPacketsType;
        typedef std::queue<ScheduledPacketItem> ScheduledPacketsType;
        typedef std::unordered_map<uint32_t, ReceivedPacketItem> ReceivedPacketsType;

        const uint8_t channel_number;

        std::optional<Timestamp> next_trigger_time;

        Callbacks callbacks;

        uint32_t next_send_id = 0;
        uint32_t expected_receive_id = 0;

        SentPacketsType sent_packets;
        ScheduledPacketsType scheduled_packets;

        ReceivedPacketsType received_packets;

        uint32_t triptime = 0;
        uint32_t triptime_middle = kStartMiddleTimeUs;

        Channel(const Channel&) = delete;
        Channel& operator=(const Channel&) = delete;

        PacketInternal CreateAckPacket(const PacketInternal& received_packet) const;

        void Reset();

        void SendTrudpPacket(PacketInternal&& packet);

        inline void UpdateTriptime(const PacketInternal& received_packet);
        inline Timestamp ExpectedTimestamp(Timestamp send_time, const PacketInternal& packet);

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
            : channel_number(settings.channel_number), next_send_id(0), expected_receive_id(0) {
            callbacks.data_received = settings.data_received_callback;
            callbacks.packet_send_requested = settings.packet_send_requested_callback;
            callbacks.channel_reset = settings.channel_reset_callback;
        }

        // inline uint32_t GetCurrentSendId() const { return next_send_id; }

        // TODO: add receive_timestamp.
        void ProcessReceivedPacket(const PacketInternal& received_packet);

        void SendData(std::vector<uint8_t>&& data);
        void OnPacketSent(Timestamp send_time, PacketInternal&& packet);

        // TODO(new): Use trudp header for unreliable packets.
        void ProcessReceivedUnreliableData(const std::vector<uint8_t>& received_data) {
            callbacks.EmitDataReceived(received_data, false);
        }

        std::optional<Timestamp> NextTriggerTime() const { return next_trigger_time; }
    };
} // namespace trudppp

#endif
