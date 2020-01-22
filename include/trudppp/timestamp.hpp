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

        inline bool operator==(const Timestamp& other) const {
            return time_point == other.time_point;
        }

        inline bool operator!=(const Timestamp& other) const {
            return time_point != other.time_point;
        }

        inline bool operator<(const Timestamp& other) const {
            return time_point < other.time_point;
        }

        inline bool operator>(const Timestamp& other) const {
            return time_point > other.time_point;
        }

        inline bool operator<=(const Timestamp& other) const {
            return time_point <= other.time_point;
        }

        inline bool operator>=(const Timestamp& other) const {
            return time_point >= other.time_point;
        }

        inline int64_t MicrosecondsSinceEpoch() const {
            return time_point.time_since_epoch().count();
        }

        inline void SetToNow() {
            using namespace std::chrono;
            time_point = time_point_cast<microseconds>(system_clock::now());
        }

        inline void ShiftMicroseconds(int64_t shift_us) {
            time_point += std::chrono::microseconds(shift_us);
        }

        inline void ShiftMilliseconds(int64_t shift_ms) {
            time_point += std::chrono::milliseconds(shift_ms);
        }

        inline void ShiftSeconds(int64_t shift_sec) {
            time_point += std::chrono::seconds(shift_sec);
        }

        inline void ShiftMinutes(int64_t shift_min) {
            time_point += std::chrono::minutes(shift_min);
        }

    private:
        // Timestamp is explicitly defined to store time in microseconds.
        std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> time_point;
    };
} // namespace trudppp

#endif
