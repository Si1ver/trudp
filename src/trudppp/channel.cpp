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

                // TODO: Debug packet dump;

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
