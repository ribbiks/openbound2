#pragma once
#include <cmath>

template<typename T>
class vec2 {
public:
    T x, y;

    vec2(T x = T{}, T y = T{}) : x(x), y(y) {}

    template<typename U>
    vec2(const vec2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    vec2(const vec2& other) : x(other.x), y(other.y) {}

    vec2(std::initializer_list<T> list) {
        auto it = list.begin();
        if (it != list.end()) {
            x = *it++;
            if (it != list.end()) {
                y = *it;
            }
        }
    }

    // equality
    bool operator==(const vec2<T>& other) const {
        return (x == other.x) && (y == other.y);
    }

    // inequality
    bool operator!=(const vec2<T>& other) const {
        return !(*this == other);
    }

    // copy assignment
    vec2& operator=(const vec2& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    // addition assignment
    vec2& operator+=(const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // subtraction assignment
    vec2& operator-=(const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // addition
    vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }

    // subtraction
    vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y - other.y);
    }

    // vec * scalar
    template<typename U>
    vec2 operator*(U scalar) const {
        return vec2(x * scalar, y * scalar);
    }

    // scalar * vec
    template<typename U>
    friend vec2 operator*(U scalar, const vec2& v) {
        return v * scalar;
    }

    T length() {
        return std::sqrt(x * x + y * y);
    }
};
