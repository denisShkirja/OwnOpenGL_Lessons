#pragma once

#include <cmath>

template <typename T>
class Vector2 {
public:
    Vector2() : x(0), y(0){};
    Vector2(T _x, T _y) : x(_x), y(_y){};

    Vector2<T> operator+(const Vector2<T> &V) {
        return Vector2<T>(x + V.x, y + V.y);
    }
    Vector2<T> operator-(const Vector2<T> &V) {
        return Vector2<T>(x - V.x, y - V.y);
    }
    Vector2<T> operator*(float f) { return Vector2<T>(x * f, y * f); }

    T x;
    T y;
};

typedef Vector2<float> Vector2f;
typedef Vector2<long> Vector2l;

template <typename T>
class Vector3 {
public:
    Vector3() : x(0), y(0), z(0){};
    Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z){};

    Vector3<T> operator+(const Vector3<T> &V) const {
        return Vector3<T>(x + V.x, y + V.y, z + V.z);
    }
    Vector3<T> operator^(const Vector3<T> &V) const {
        return Vector3<T>(y * V.z - z * V.y, z * V.x - x * V.z,
                          x * V.y - y * V.x);
    }
    Vector3<T> operator-(const Vector3<T> &V) const {
        return Vector3<T>(x - V.x, y - V.y, z - V.z);
    }
    T operator*(const Vector3<T> &V) const {
        return x * V.x + y * V.y + z * V.z;
    }
    Vector3<T> operator*(float f) const {
        return Vector3<T>(x * f, y * f, z * f);
    }

    float len() const { return std::sqrt(x * x + y * y + z * z); }
    void normalize(T l = 1) { *this = (*this) * (l / len()); }

    T x;
    T y;
    T z;
};

typedef Vector3<float> Vector3f;
typedef Vector3<long> Vector3l;

