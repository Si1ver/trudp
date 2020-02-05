#pragma once

#ifndef TRUDPPP_SERVICE_HPP
#define TRUDPPP_SERVICE_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "trudppp/callbacks.hpp"
#include "trudppp/channel.hpp"
#include "trudppp/connection.hpp"
#include "trudppp/packet.hpp"

namespace trudppp {

    template <class TEndpoint>
    class Service {
    public:
        struct Settings {
            bool accept_incoming_connections;

            std::function<void(Connection&)> connection_initialized_callback;

            std::function<void(Connection&)> connection_destroyed_callback;

            std::function<void(Connection& connection, uint8_t channel_number,
                const std::vector<uint8_t>& received_data, bool is_reliable)>
                data_received_callback;

            std::function<void(Connection& connection, uint8_t channel_number,
                Packet&& packet_to_send)>
                data_send_requested_callback;
        };

    private:
        Callbacks callbacks;

        std::unordered_map<TEndpoint, Connection> connections;

        bool accept_incoming_connections;

        Service(const Service&) = delete;

        Service& operator=(const Service&) = delete;

    public:
        Service(const Settings& settings) {
            accept_incoming_connections = settings.accept_incoming_connections;
            callbacks.connection_initialized = settings.connection_initialized_callback;
            callbacks.connection_destroyed = settings.connection_destroyed_callback;
            callbacks.data_received = settings.data_received_callback;
            callbacks.data_send_requested = settings.data_send_requested_callback;
        }

        Callbacks& GetCallbacks() { return callbacks; }

        void ProcessReceivedData(
            const TEndpoint& endpoint, const std::vector<uint8_t>& received_data) {
            auto existing_connection_it = connections.find(endpoint);

            if (existing_connection_it != connections.end()) {
                existing_connection_it->second.ProcessReceivedData(received_data);
            } else if (accept_incoming_connections) {
                auto emplace_result = connections.emplace(endpoint, callbacks);
                emplace_result.first->second.ProcessReceivedData(received_data);
            } else {
                // Data received from unknown endpoint is dropped because
                // accept_incoming_connections is set to false.
            }
        }
    };
} // namespace trudppp

#endif
