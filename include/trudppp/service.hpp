#pragma once

#ifndef TRUDPPP_SERVICE_HPP
#define TRUDPPP_SERVICE_HPP

#include <functional>
#include <unordered_map>

#include "trudppp/channel.hpp"
#include "trudppp/connection.hpp"

namespace trudppp {
    template <template <class> class TCallback = std::function>
    struct ServiceSettings {
        bool accept_incoming_connections;

        TCallback<void(Connection<TCallback>&)> connection_initialized_callback;

        TCallback<void(Connection<TCallback>&)> connection_destroyed_callback;
    };

    template <class TEndpoint, template <class> class TCallback = std::function>
    class Service {
    private:
        std::unordered_map<TEndpoint, Connection<TCallback>> connections;

        bool accept_incoming_connections;

        Callbacks<TCallback> callbacks;

        Service(const Service&) = delete;

        Service& operator=(const Service&) = delete;

    public:
        Service(const ServiceSettings<TCallback>& settings) {
            accept_incoming_connections = settings.accept_incoming_connections;
            callbacks.connection_initialized = settings.connection_initialized_callback;
            callbacks.connection_destroyed = settings.connection_destroyed_callback;
        }

        Callbacks<TCallback>& GetCallbacks() { return callbacks; }

        void ProcessReceivedData(const TEndpoint& endpoint, const std::vector<uint8_t>& data) {
            auto existing_connection_it = connections.find(endpoint);

            if (existing_connection_it != connections.end()) {
                existing_connection_it->second.ProcessReceivedData(data);
            } else if (accept_incoming_connections) {
                auto emplace_result = connections.emplace(endpoint, callbacks);
                emplace_result.first->second.ProcessReceivedData(data);
            } else {
                // Data received from unknown endpoint is dropped because
                // accept_incoming_connections is set to false.
            }
        }
    };
} // namespace trudppp

#endif
