#include "trudppp/callbacks.hpp"
#include "trudppp/channel.hpp"
#include "trudppp/connection.hpp"
#include "trudppp/constants.hpp"
#include "trudppp/packet.hpp"
#include "trudppp/service.hpp"

#include <iostream>

const char* include_callbacks_stub();
const char* include_channel_stub();
const char* include_connection_stub();
const char* include_constants_stub();
const char* include_packet_stub();
const char* include_service_stub();

int main() {
    std::cout << include_callbacks_stub() << std::endl;
    std::cout << include_channel_stub() << std::endl;
    std::cout << include_connection_stub() << std::endl;
    std::cout << include_constants_stub() << std::endl;
    std::cout << include_packet_stub() << std::endl;
    std::cout << include_service_stub() << std::endl;

    return 0;
}
