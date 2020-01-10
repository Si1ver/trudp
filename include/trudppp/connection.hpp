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

        Channel& GetOrCreateChannel(int channel_number);

    public:
        Connection(const Callbacks& callbacks) : callbacks(callbacks) {
            callbacks.EmitConnectionInitialized(*this);
        }

        ~Connection() {
            callbacks.EmitConnectionDestroyed(*this);

            // TODO: destroy channels.
        }

        void ProcessReceivedData(const std::vector<uint8_t>& received_data);
    };
} // namespace trudppp

#endif
