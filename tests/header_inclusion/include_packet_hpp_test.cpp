// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/packet.hpp"

char* include_packet_stub() {
    return "trudppp/packet.hpp";
}
