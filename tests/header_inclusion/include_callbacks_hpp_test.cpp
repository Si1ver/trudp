// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/callbacks.hpp"

char* include_callbacks_stub() {
    return "trudppp/callbacks.hpp";
}
