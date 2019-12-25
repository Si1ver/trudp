#pragma once

#ifndef TRUDPPP_CALLBACKS_HPP
#define TRUDPPP_CALLBACKS_HPP

#include <functional>

namespace trudppp {
    template <template <class> class TCallback = std::function>
    class Callbacks {
    public:
        TCallback<void()> connection_initialized;

        TCallback<void()> connection_destroyed;
    };
} // namespace trudppp

#endif
