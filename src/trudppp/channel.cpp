#include "trudppp/channel.hpp"

#include <stdexcept>

#include "trudppp/serialized_packet.hpp"

namespace trudppp {
    void Channel::RequestSendPacket(const Packet& packet_to_send) {
        const std::vector<uint8_t> serialized_packet = internal::SerializePacket(packet_to_send);

        EmitSendDataRequestCallback(serialized_packet);
    }

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

                RequestSendPacket(ack_packet);

                // TODO: Event: ping received.

                break;
            }

            case PacketType::Data: {
                Packet ack_packet = CreateAckPacket(received_packet);

                RequestSendPacket(ack_packet);

                // TODO: Debug packet dump.

                // TODO: Receive queue.

                uint32_t packet_id = received_packet.GetId();

                // This check is ported from legacy code.
                if (expected_receive_id == 0 && packet_id != 0) {
                    // TODO: Reset channel.
                    break;
                }

                if (expected_receive_id == packet_id) {
                    EmitPacketReceivedCallback(received_packet);

                    ++expected_receive_id;

                    // Check received packets with bigger id.
                    if (!received_packets.empty()) {
                        auto existing_item = received_packets.find(expected_receive_id);

                        while (existing_item != received_packets.end()) {
                            EmitPacketReceivedCallback(existing_item->second.packet);

                            received_packets.erase(existing_item);

                            ++expected_receive_id;
                            existing_item = received_packets.find(expected_receive_id);
                        }
                    }

                    break;
                }

                // TODO: replace with modulus substraction
                if (expected_receive_id < packet_id) {
                    auto existing_item = received_packets.find(packet_id);

                    if (existing_item == received_packets.end()) {
                        auto receive_timestamp_us =
                            std::chrono::time_point_cast<std::chrono::microseconds>(
                                std::chrono::system_clock::now());

                        received_packets.emplace(std::piecewise_construct,
                            std::make_tuple(packet_id),
                            std::forward_as_tuple(receive_timestamp_us, received_packet));
                    }
                }

                break;
            }

            case PacketType::Reset: {
                Packet ack_packet = CreateAckPacket(received_packet);

                RequestSendPacket(ack_packet);

                // TODO: Event: Channel will be reset.

                // TODO: Reset channel (or connection?).

                break;
            }

            default: {
                break;
            }
        }
    }
} // namespace trudppp
