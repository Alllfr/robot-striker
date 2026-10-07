#pragma once
#include <vector>

#include "Ball.hpp"
#include "Field.hpp"
#include "MathUtils.hpp"

struct Pose {
    math::Vec2 position;
    double headingDeg = 0.0;
};

// Data bola menurut kamera. Hanya ini yang boleh dipakai robot (tidak boleh membaca Ball langsung).
struct BallObservation {
    bool visible = false;
    math::Vec2 relative;       // x = depan, y = kiri (meter), resolusi 1 petak
    double distance = 0.0;
    double bearingDeg = 0.0;   // relatif terhadap heading, -180..180
};

struct Perception {
    BallObservation ball;
    std::vector<Cell> visibleCells;  // area yang tertangkap kamera (digambar '@')
};

// Kamera segitiga: apex di robot, tinggi 1.5 m ke depan, alas 3.5 m.
class Sensor {
public:
    explicit Sensor(double baseLength = 3.5, double range = 1.5);

    bool isInView(const Pose& pose, const math::Vec2& worldPoint) const;
    std::vector<Cell> visibleCells(const Field& field, const Pose& pose) const;
    Perception capture(const Field& field, const Pose& pose, const Ball& ball) const;

private:
    double baseLength_;
    double range_;
};
