#include "trudppp/packet.hpp"

namespace trudppp
{
    uint32_t SequenceId::ModuleSub(uint64_t id_a, uint64_t id_b, uint64_t mod) {
        int64_t sub = (id_a % mod) + mod - (id_b % mod);
        return (uint64_t)(sub % mod);
    }

    bool SequenceId::operator<(SequenceId rhs) const {
        uint32_t diff = ModuleSub(rhs.id, id, SequenceId::packet_id_limit);

        return (diff < (SequenceId::packet_id_limit / 2)) || (diff - SequenceId::packet_id_limit) > 0;
    }
}
