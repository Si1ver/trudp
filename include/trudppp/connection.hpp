#pragma once

#ifndef TRUDPPP_CONNECTION_HPP
#define TRUDPPP_CONNECTION_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "trudppp/callbacks.hpp"
#include "trudppp/channel.hpp"

namespace trudppp {
    class Connection {
    private:
        const Callbacks& callbacks;

        std::unordered_map<int, Channel> channels;

        // TODO: stat.

        Connection(const Connection&) = delete;

        Connection& operator=(const Connection&) = delete;

        inline void EmitInitializeCallback() {
            if (callbacks.connection_initialized) {
                callbacks.connection_initialized(*this);
            }
        }

        inline void EmitDestroyCallback() {
            if (callbacks.connection_destroyed) {
                callbacks.connection_destroyed(*this);
            }
        }

        Channel& GetOrCreateChannel(int channel_number) {
            auto existing_channel_it = channels.find(channel_number);

            if (existing_channel_it != channels.end()) {
                return existing_channel_it->second;
            } else {
                // This code creates new Channel instance in map,
                // passing needed values to Channel's constructor.
                auto emplace_result = channels.emplace(std::piecewise_construct,
                    std::make_tuple(channel_number), std::forward_as_tuple(callbacks, *this));
                return emplace_result.first->second;
            }
        }

    public:
        Connection(const Callbacks& callbacks) : callbacks(callbacks) { EmitInitializeCallback(); }

        ~Connection() {
            EmitDestroyCallback();

            // destroy channels
        }

        void ProcessReceivedData(const std::vector<uint8_t>& received_data);
    };
} // namespace trudppp

#endif
