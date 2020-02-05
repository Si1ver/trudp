#include "trudppp/connection.hpp"

#include <cstdint>
#include <vector>

#include "trudppp/serialized_packet.hpp"
#include "trudppp/packet.hpp"

namespace trudppp {
    Channel& Connection::GetOrCreateChannel(int channel_number) {
        auto existing_channel_it = channels.find(channel_number);

        if (existing_channel_it != channels.end()) {
            return existing_channel_it->second;
        } else {
            Channel::Settings settings;
            settings.channel_number = channel_number;
            settings.data_received_callback = [channel_number, this](
                                                  const std::vector<uint8_t>& received_data,
                                                  bool is_reliable) {
                callbacks.EmitDataReceived(*this, channel_number, received_data, is_reliable);
            };

            settings.packet_send_requested_callback = [channel_number, this](
                                                          PacketInternal&& packet_to_send) {
                callbacks.EmitDataSendRequested(*this, channel_number, Packet(std::move(packet_to_send)));
            };

            auto emplace_result = channels.emplace(channel_number, settings);
            return emplace_result.first->second;
        }
    }

    Channel* Connection::GetChannel(int channel_number) {
        auto existing_channel_it = channels.find(channel_number);

        return existing_channel_it == std::end(channels) ? nullptr : &(existing_channel_it->second);
    }

    void Connection::ProcessReceivedData(const std::vector<uint8_t>& received_data) {
        if (internal::CheckBufferIsValidPacket(received_data)) {
            const PacketInternal& received_packet = internal::DeserializePacket(received_data);

            int channel_number = received_packet.GetChannelNumber();

            // This check is ported from legacy code.
            if (received_packet.GetType() == trudppp::PacketType::Ping) {
                auto existing_channel_it = channels.find(channel_number);

                if (existing_channel_it == channels.end()) {
                    return;
                }
            }

            Channel& channel = GetOrCreateChannel(channel_number);

            channel.ProcessReceivedPacket(received_packet);
        } else {
            // Received data is not a valid trudp packet.
            // Assuming it is unreliable packet for channel 0.
            const int channel_number = 0;

            Channel& channel = GetOrCreateChannel(channel_number);

            channel.ProcessReceivedUnreliableData(received_data);
        }
    }

    void Connection::SendData(int channel_number, std::vector<uint8_t>&& data) {
        auto& channel = GetOrCreateChannel(channel_number);

        channel.SendData(std::move(data));
    }

    void Connection::OnPacketSent(Timestamp send_time, Packet&& packet) {
        auto channel = GetChannel(packet.packet.GetChannelNumber());

        if (channel == nullptr) {
            //TODO: smth went terribly wrong
            return;
        }

        channel->OnPacketSent(send_time, std::move(packet.packet));
    }
} // namespace trudppp
