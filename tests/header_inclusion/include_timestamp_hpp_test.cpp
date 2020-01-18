// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/timestamp.hpp"

const char* include_timestamp_stub() {
    return "trudppp/timestamp.hpp";
}
