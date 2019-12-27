// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/channel.hpp"

char* include_channel_stub() {
    return "trudppp/channel.hpp";
}
