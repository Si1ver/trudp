#include "trudppp/connection.hpp"

#include <cstdint>
#include <vector>

#include "trudppp/serialized_packet.hpp"

namespace trudppp {
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
