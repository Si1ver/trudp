#pragma once

#ifndef TRUDPPP_CONSTANTS_HPP
#define TRUDPPP_CONSTANTS_HPP

#include <cstddef>
#include <cstdint>

namespace trudppp {
    static constexpr int kTrudpppProtocolVersion = 2;
    static constexpr size_t kMinimumSerializedMessageSize = 12;
    static constexpr uint32_t kMaxRTTUs = 500000;
    static constexpr uint32_t kStartMiddleTimeUs = (kMaxRTTUs / 5);
    static constexpr float kTriptimeFactor = 1.5;
    static constexpr float kMaxTriptimeMiddle =  5757575.f / 2.f;
    static constexpr uint8_t kMaxSentPackets = 40;
    static constexpr uint16_t kRTTMagicNumberUs = 30000;
} // namespace trudppp

#endif
