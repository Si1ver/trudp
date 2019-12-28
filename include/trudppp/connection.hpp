#pragma once

#ifndef TRUDPPP_CONNECTION_HPP
#define TRUDPPP_CONNECTION_HPP

#include <functional>
#include <map>
#include <string>

#include "trudppp/callbacks.hpp"
#include "trudppp/channel.hpp"

namespace trudppp {
    template <template <class> class TCallback = std::function>
    class Connection {
    private:
        std::map<std::string, Channel<TCallback>> channels;

        const Callbacks<TCallback>& callbacks;

        // TODO: stat.

        Connection(const Connection&) = delete;

        Connection& operator=(const Connection&) = delete;

        void EmitInitializeCallback() {
            if (callbacks.connection_initialized) {
                callbacks.connection_initialized(&this);
            }
        }

        void EmitDestroyCallback() {
            if (callbacks.connection_destroyed) {
                callbacks.connection_destroyed(&this);
            }
        }

    public:
        Connection(const Callbacks<TCallback>& callbacks) : callbacks(callbacks) {
            EmitInitializeCallback();
        }

        ~Connection() {
            EmitDestroyCallback();

            // destroy channels
        }

    };
} // namespace trudppp

#endif
