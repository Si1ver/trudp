#include "trudppp/connection.hpp"

#include <cstdint>
#include <vector>

#include "trudppp/serialized_packet.hpp"

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
                                                          const Packet& packet_to_send) {
                std::vector<uint8_t> serialized_packet = internal::SerializePacket(packet_to_send);
                callbacks.EmitDataSendRequested(*this, channel_number, serialized_packet);
            };

            auto emplace_result = channels.emplace(channel_number, settings);
            return emplace_result.first->second;
        }
    }

    void Connection::ProcessReceivedData(const std::vector<uint8_t>& received_data) {
        if (internal::CheckBufferIsValidPacket(received_data)) {
            const Packet& received_packet = internal::DeserializePacket(received_data);

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
} // namespace trudppp
