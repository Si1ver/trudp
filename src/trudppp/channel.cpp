#include "trudppp/channel.hpp"

#include <stdexcept>
#include <utility>

namespace trudppp {
    PacketInternal Channel::CreateAckPacket(const PacketInternal& received_packet) const {
        switch (received_packet.GetType()) {
            case PacketType::Ping: {
                return PacketInternal(PacketType::AckOnPing, received_packet.GetChannelNumber(),
                    received_packet.GetId(), received_packet.GetData(),
                    received_packet.GetTimestamp());
            }

            case PacketType::Data: {
                return PacketInternal(PacketType::Ack, received_packet.GetChannelNumber(),
                    received_packet.GetId(), received_packet.GetTimestamp());
            }

            case PacketType::Reset: {
                return PacketInternal(PacketType::AckOnReset, received_packet.GetChannelNumber(),
                    received_packet.GetId(), received_packet.GetTimestamp());
            }

            default:
                throw std::invalid_argument("Failed to create Ack packet: invalid packet type.");
        }
    }

    void Channel::UpdateTriptime(const PacketInternal& received_packet) {
        auto now = Timestamp();
        triptime = now.MicrosecondsSinceEpoch() - received_packet.GetTimestamp().MicrosecondsSinceEpoch();

        // Calculate and set Middle Triptime value
        triptime_middle = triptime_middle == kStartMiddleTimeUs
            ? triptime * kTriptimeFactor
            : // Set first middle time
            triptime > triptime_middle ? triptime * kTriptimeFactor
                                                : // Set middle time to max triptime
                (triptime_middle * 19 + triptime) / 20.0; // Calculate middle value

        // Correct triptimeMiddle
        if (triptime_middle < triptime * kTriptimeFactor)
            triptime_middle = triptime * kTriptimeFactor;
        if (triptime_middle > triptime * 10)
            triptime_middle = triptime * 10;
        if (triptime_middle > kMaxTriptimeMiddle)
            triptime_middle = kMaxTriptimeMiddle;

        //TODO: stats
    }

    Timestamp Channel::ExpectedTimestamp(Timestamp send_time, const PacketInternal& packet) {
        auto rtt = std::min(kMaxRTTUs, triptime_middle + kRTTMagicNumberUs);

        send_time.ShiftMicroseconds(rtt);

        return send_time;
    }

    void Channel::Reset() {
        callbacks.EmitChannelReset();

        next_send_id = 0;
        expected_receive_id = 0;

        // Clear saved packets.
        SentPacketsType new_sent_packets;
        std::swap(sent_packets, new_sent_packets);

        ScheduledPacketsType new_scheduled_packets;
        std::swap(scheduled_packets, new_scheduled_packets);

        ReceivedPacketsType new_received_packets;
        std::swap(received_packets, new_received_packets);
    }

    void Channel::ProcessReceivedPacket(const PacketInternal& received_packet) {
        switch (received_packet.GetType()) {
            case PacketType::Ack: {
                auto sent_packet_it = std::find_if(std::begin(sent_packets), std::end(sent_packets),
                    [packet_id = received_packet.GetId()](const SentPacketsType::value_type& sent_packet) {
                        return sent_packet.packet.GetId();
                    });

                if (sent_packet_it == std::end(sent_packets)) return;//TODO:???

                callbacks.EmitAckReceived(received_packet.GetId());

                sent_packets.erase(sent_packet_it);

                // if we got ack for 0 packet, we can send up to kMaxSentPackets packets
                // else we have one more place in send queue, so take only one packet from write queue
                auto max_scheduled_packets_sent = received_packet.GetId() == 0 ? kMaxSentPackets : 1;
                auto scheduled_packets_sent = 0;

                while (!scheduled_packets.empty() && scheduled_packets_sent < max_scheduled_packets_sent) {
                    auto scheduled_packet = std::move(scheduled_packets.front());
                    scheduled_packets.pop();

                    SendTrudpPacket(std::move(scheduled_packet.packet));
                    scheduled_packets_sent++;
                }

                UpdateTriptime(received_packet);

                next_trigger_time = sent_packets.empty() ? std::optional<Timestamp>{} :
                    sent_packets.front().expected_time;
                //TODO: stat
                break;
            }

            case PacketType::AckOnReset: {
                break;
            }

            case PacketType::AckOnPing: {
                break;
            }

            case PacketType::Ping: {
                PacketInternal ack_packet = CreateAckPacket(received_packet);
                callbacks.EmitSendPacketRequested(std::move(ack_packet));

                // TODO: Event: ping received.

                break;
            }

            case PacketType::Data: {
                PacketInternal ack_packet = CreateAckPacket(received_packet);
                callbacks.EmitSendPacketRequested(std::move(ack_packet));

                // TODO: Debug packet dump.

                // TODO: Receive queue.

                uint32_t packet_id = received_packet.GetId();

                // This check is ported from legacy code.
                if (expected_receive_id == 0 && packet_id != 0) {
                    Reset();
                    break;
                }

                if (expected_receive_id == packet_id) {
                    callbacks.EmitDataReceived(received_packet.GetData(), true);

                    expected_receive_id = IncrementPacketId(expected_receive_id);

                    // Check already received packets.
                    if (!received_packets.empty()) {
                        auto existing_item = received_packets.find(expected_receive_id);

                        while (existing_item != received_packets.end()) {
                            const PacketInternal& packet = existing_item->second.packet;
                            callbacks.EmitDataReceived(packet.GetData(), true);

                            received_packets.erase(existing_item);

                            expected_receive_id = IncrementPacketId(expected_receive_id);
                            existing_item = received_packets.find(expected_receive_id);
                        }
                    }

                    break;
                }

                // TODO: replace with modulus subtraction
                if (expected_receive_id < packet_id) {
                    auto existing_item = received_packets.find(packet_id);

                    if (existing_item == received_packets.end()) {
                        Timestamp receive_timestamp;

                        received_packets.emplace(std::piecewise_construct,
                            std::make_tuple(packet_id),
                            std::forward_as_tuple(receive_timestamp, received_packet));
                    }

                    break;
                }

                // This check is ported from legacy code.
                if (packet_id == 0 && expected_receive_id != 1) {
                    Reset();
                    break;
                }

                break;
            }

            case PacketType::Reset: {
                PacketInternal ack_packet = CreateAckPacket(received_packet);
                callbacks.EmitSendPacketRequested(std::move(ack_packet));

                Reset();

                break;
            }

            default: {
                break;
            }
        }
    }

    void Channel::SendData(std::vector<uint8_t>&& received_data) {
        //TODO: maybe we can move data here?
        auto packet = PacketInternal(PacketType::Data, channel_number, next_send_id, std::move(received_data), Timestamp());

        next_send_id = IncrementPacketId(next_send_id);

        SendTrudpPacket(std::move(packet));
    }

    void Channel::SendTrudpPacket(PacketInternal&& packet) {
        auto current_send_queue_size = sent_packets.size();

        bool send_now = current_send_queue_size < kMaxSentPackets;

        // if we are still connecting, do not send anything
        if (current_send_queue_size == 1 && sent_packets.front().packet.GetId() == 0) {
            send_now = false;
        }

        if (send_now) {
            callbacks.EmitSendPacketRequested(std::move(packet));
        } else {
            scheduled_packets.emplace(std::move(packet));
        }

        //TODO: stat
    }

    void Channel::OnPacketSent(Timestamp send_time, PacketInternal&& packet) {
        auto expected_timestamp = ExpectedTimestamp(send_time, packet);

        SentPacketItem sent_packet(std::move(packet), expected_timestamp);

        if (!next_trigger_time.has_value()) {
            next_trigger_time = expected_timestamp;
        }

        sent_packets.emplace_back(std::move(sent_packet));
    }
} // namespace trudppp
