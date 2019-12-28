#pragma once

#ifndef TRUDPPP_CALLBACKS_HPP
#define TRUDPPP_CALLBACKS_HPP

#include <functional>

namespace trudppp {
    template <template <class> class TCallback>
    class Connection;

    template <template <class> class TCallback = std::function>
    class Callbacks {
    public:
        TCallback<Connection<TCallback>&> connection_initialized;

        TCallback<Connection<TCallback>&> connection_destroyed;
    };
} // namespace trudppp

#endif
