// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/connection.hpp"

const char* include_connection_stub() {
    return "trudppp/connection.hpp";
}
