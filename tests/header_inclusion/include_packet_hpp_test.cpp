// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/packet.hpp"

const char* include_packet_stub() {
    return "trudppp/packet.hpp";
}
