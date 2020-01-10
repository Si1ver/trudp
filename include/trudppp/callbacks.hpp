#pragma once

#ifndef TRUDPPP_CALLBACKS_HPP
#define TRUDPPP_CALLBACKS_HPP

#include <cstdint>
#include <functional>
#include <vector>

namespace trudppp {
    // Forward declaration of classes used in callbacks.
    class Packet;
    class Connection;

    class Callbacks {
    public:
        typedef void ConnectionInitializedCallback(Connection& connection);
        typedef void ConnectionDestroyedCallback(Connection& connection);
        typedef void DataReceivedCallback(Connection& connection, uint8_t channel_number,
            const std::vector<uint8_t>& received_data, bool is_reliable);
        typedef void DataSendRequestedCallback(Connection& connection, uint8_t channel_number,
            const std::vector<uint8_t>& data_to_send);

        std::function<ConnectionInitializedCallback> connection_initialized;
        std::function<ConnectionDestroyedCallback> connection_destroyed;
        std::function<DataReceivedCallback> data_received;
        std::function<DataSendRequestedCallback> data_send_requested;

        inline void EmitConnectionInitialized(Connection& connection) const {
            if (connection_initialized) {
                connection_initialized(connection);
            }
        }

        inline void EmitConnectionDestroyed(Connection& connection) const {
            if (connection_destroyed) {
                connection_destroyed(connection);
            }
        }

        inline void EmitDataReceived(Connection& connection, uint8_t channel_number,
            const std::vector<uint8_t>& received_data, bool is_reliable) const {
            if (data_received) {
                data_received(connection, channel_number, received_data, is_reliable);
            }
        }

        inline void EmitDataSendRequested(Connection& connection, uint8_t channel_number,
            const std::vector<uint8_t>& data_to_send) const {
            if (data_send_requested) {
                data_send_requested(connection, channel_number, data_to_send);
            }
        }
    };
} // namespace trudppp

#endif
