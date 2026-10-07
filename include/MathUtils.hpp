#pragma once
// Modul helper matematika (prinsip DRY): semua hitungan jarak 2D, bearing,
// dan normalisasi sudut ada di sini, tidak boleh ditulis ulang di tempat lain.
#include <cmath>

namespace math {

constexpr double kPi = 3.14159265358979323846;

inline double toRadians(double deg) { return deg * kPi / 180.0; }
inline double toDegrees(double rad) { return rad * 180.0 / kPi; }

inline bool nearlyEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) <= eps;
}

// Normalisasi sudut ke rentang (-180, 180] derajat.
inline double normalizeAngle(double deg) {
    deg = std::fmod(deg, 360.0);
    if (deg > 180.0) deg -= 360.0;
    else if (deg <= -180.0) deg += 360.0;
    return deg;
}

struct Vec2 {
    double x = 0.0;
    double y = 0.0;

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(double k) const { return {x * k, y * k}; }
    double dot(const Vec2& o) const { return x * o.x + y * o.y; }
    double length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        double len = length();
        return len < 1e-12 ? Vec2{0.0, 0.0} : Vec2{x / len, y / len};
    }
};

// Jarak Euclidean dua titik.
inline double distance(const Vec2& a, const Vec2& b) { return (a - b).length(); }

// Sudut global (derajat, 0 = sumbu +x, berlawanan jarum jam) dari titik `from` ke `to`.
inline double bearingDeg(const Vec2& from, const Vec2& to) {
    Vec2 d = to - from;
    return toDegrees(std::atan2(d.y, d.x));
}

// Vektor satuan dari sudut (derajat).
inline Vec2 fromAngle(double deg) {
    double r = toRadians(deg);
    return {std::cos(r), std::sin(r)};
}

// Rotasi vektor sebesar `deg` derajat (berlawanan jarum jam).
inline Vec2 rotate(const Vec2& v, double deg) {
    double r = toRadians(deg);
    double c = std::cos(r), s = std::sin(r);
    return {v.x * c - v.y * s, v.x * s + v.y * c};
}

// Frame lokal robot: x = depan, y = kiri.
inline Vec2 worldToLocal(const Vec2& worldDelta, double headingDeg) {
    return rotate(worldDelta, -headingDeg);
}
inline Vec2 localToWorld(const Vec2& local, double headingDeg) {
    return rotate(local, headingDeg);
}

}  // namespace math
