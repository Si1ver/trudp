#pragma once

#ifndef TRUDPPP_TIMESTAMP_HPP
#define TRUDPPP_TIMESTAMP_HPP

#include <chrono>
#include <cstdint>

namespace trudppp {
    class Timestamp {
    public:
        Timestamp() { SetToNow(); }

        Timestamp(const Timestamp& other) { time_point = other.time_point; }

        Timestamp& operator=(const Timestamp& other) { time_point = other.time_point; }

        bool operator==(const Timestamp& other) const { return time_point == other.time_point; }

        int64_t MicrosecondsSinceEpoch() const { return time_point.time_since_epoch().count(); }

        void SetToNow() {
            using namespace std::chrono;
            time_point = time_point_cast<microseconds>(system_clock::now());
        }

        void ShiftMicroseconds(int64_t shift_us) {
            time_point += std::chrono::microseconds(shift_us);
        }

        void ShiftMilliseconds(int64_t shift_ms) {
            time_point += std::chrono::milliseconds(shift_ms);
        }

        void ShiftSeconds(int64_t shift_sec) { time_point += std::chrono::seconds(shift_sec); }

        void ShiftMinutes(int64_t shift_min) { time_point += std::chrono::minutes(shift_min); }

    private:
        std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> time_point;
    };
} // namespace trudppp

#endif
