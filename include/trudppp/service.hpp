#pragma once

#ifndef TRUDPPP_SERVICE_HPP
#define TRUDPPP_SERVICE_HPP

#include <functional>
#include <unordered_map>

#include "trudppp/connection.hpp"

namespace trudppp {
    template <template <class> class TCallback = std::function>
    struct ServiceSettings {
        TCallback<void()> connection_initialized_callback;

        TCallback<void()> connection_destroyed_callback;
    };

    template <class TEndpoint, template <class> class TCallback = std::function>
    class Service {
    private:
        std::unordered_map<TEndpoint, Connection<TCallback>> connections;

        Callbacks<TCallback> callbacks;

        Service(const Service&) = delete;

        Service& operator=(const Service&) = delete;

    public:
        Service(const ServiceSettings<TCallback>& settings) {
            callbacks.connection_initialized = settings.connection_initialized_callback;
            callbacks.connection_destroyed = settings.connection_destroyed_callback;
        }

        Callbacks<TCallback>& GetCallbacks() { return callbacks; }

        void ProcessReceivedData(const TEndpoint& endpoint, const std::vector<uint8_t>& data) {
        }
    };
} // namespace trudppp

#endif
