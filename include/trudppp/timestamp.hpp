#pragma once

#ifndef TRUDPPP_TIMESTAMP_HPP
#define TRUDPPP_TIMESTAMP_HPP

#include <chrono>
#include <cstdint>

namespace trudppp {
    class Timestamp {
    public:
        Timestamp(const Timestamp& other) { time_point = other.time_point; }

        Timestamp& operator=(const Timestamp& other) { time_point = other.time_point; return *this; }

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

        inline static Timestamp Now() {
            using namespace std::chrono;
            return Timestamp(time_point_cast<microseconds>(system_clock::now()));
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
        using TimestampTimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

        TimestampTimePoint time_point;

        // Default constructor is deleted to prevent accidental creation of new timestamp value.
        Timestamp() = delete;

        Timestamp(TimestampTimePoint time_point_) {
            time_point = time_point_;
        }
    };
} // namespace trudppp

#endif
