#include "trudppp/channel.hpp"

#include <stdexcept>
#include <utility>

namespace trudppp {
    Packet Channel::CreateAckPacket(const Packet& received_packet) const {
        switch (received_packet.GetType()) {
            case PacketType::Ping: {
                return std::move(Packet(PacketType::AckOnPing, received_packet.GetChannelNumber(),
                    received_packet.GetId(), received_packet.GetData(),
                    received_packet.GetTimestamp()));
            }

            case PacketType::Data: {
                return std::move(Packet(PacketType::Ack, received_packet.GetChannelNumber(),
                    received_packet.GetId(), received_packet.GetTimestamp()));
            }

            case PacketType::Reset: {
                return std::move(Packet(PacketType::AckOnReset, received_packet.GetChannelNumber(),
                    received_packet.GetId(), received_packet.GetTimestamp()));
            }

            default:
                throw std::invalid_argument("Failed to create Ack packet: invalid packet type.");
        }
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

    void Channel::ProcessReceivedPacket(const Packet& received_packet) {
        switch (received_packet.GetType()) {
            case PacketType::Ack: {
                break;
            }

            case PacketType::AckOnReset: {
                break;
            }

            case PacketType::AckOnPing: {
                break;
            }

            case PacketType::Ping: {
                Packet ack_packet = CreateAckPacket(received_packet);
                callbacks.EmitSendPacketRequested(ack_packet);

                // TODO: Event: ping received.

                break;
            }

            case PacketType::Data: {
                Packet ack_packet = CreateAckPacket(received_packet);
                callbacks.EmitSendPacketRequested(ack_packet);

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
                            const Packet& packet = existing_item->second.packet;
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
                Packet ack_packet = CreateAckPacket(received_packet);
                callbacks.EmitSendPacketRequested(ack_packet);

                Reset();

                break;
            }

            default: {
                break;
            }
        }
    }

    void Channel::SendData(const std::vector<uint8_t>& received_data) {

    }
} // namespace trudppp
