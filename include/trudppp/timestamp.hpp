#pragma once

#ifndef TRUDPPP_TIMESTAMP_HPP
#define TRUDPPP_TIMESTAMP_HPP

#include <chrono>
#include <cstdint>

namespace trudppp {
    class Duration {
    public:
        inline int64_t Microseconds() const {
            return microseconds.count();
        }

        inline std::chrono::microseconds Value() const {
            return microseconds;
        }

        inline static Duration Microseconds(int64_t microseconds) {
            return Duration(std::chrono::microseconds(microseconds));
        }

        inline static Duration Milliseconds(int64_t milliseconds) {
            auto chrono_milliseconds = std::chrono::milliseconds(milliseconds);
            auto microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>(chrono_milliseconds);
            return Duration(microseconds);
        } 

        inline static Duration Seconds(int64_t seconds) {
            auto chrono_seconds = std::chrono::seconds(seconds);
            auto microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>(chrono_seconds);
            return Duration(microseconds);
        } 

        inline static Duration Minutes(int64_t minutes) {
            auto chrono_minutes = std::chrono::milliseconds(minutes);
            auto microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>(chrono_minutes);
            return Duration(microseconds);
        } 

    private:
        std::chrono::microseconds microseconds;

        Duration() = delete;

        explicit Duration(std::chrono::microseconds microseconds_) : microseconds(microseconds_) {}
    };

    class Timestamp {
    public:
        Timestamp(const Timestamp& other) : time_point(other.time_point) {}

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

        inline Timestamp operator+(const Duration& duration) {
            return Timestamp(time_point + duration.Value());
        }

        inline Timestamp operator-(const Duration& duration) {
            return Timestamp(time_point - duration.Value());
        }

        inline Duration operator-(const Timestamp& other) {
            auto time_duration = time_point - other.time_point;
            return Duration::Microseconds(time_duration.count());
        }

        inline int64_t MicrosecondsSinceEpoch() const {
            return time_point.time_since_epoch().count();
        }

        inline static Timestamp Now() {
            using namespace std::chrono;
            return Timestamp(time_point_cast<microseconds>(system_clock::now()));
        }

    private:
        // Timestamp is explicitly defined to store time in microseconds.
        using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

        TimePoint time_point;

        // Default constructor is deleted to prevent accidental creation of new timestamp value.
        Timestamp() = delete;

        Timestamp(TimePoint time_point_) : time_point(time_point_) {}
    };
} // namespace trudppp

#endif
