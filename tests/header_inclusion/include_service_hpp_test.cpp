// This test ensures that header inclusion does not cause any compilation errors.

#include "trudppp/service.hpp"

char* include_service_stub() {
    return "trudppp/service.hpp";
}
