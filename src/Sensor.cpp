#include "Sensor.hpp"

#include <cmath>

Sensor::Sensor(double baseLength, double range) : baseLength_(baseLength), range_(range) {}

// Titik dalam segitiga jika: 0 < depan <= range  dan  |kiri| <= (alas/2) * depan/range
bool Sensor::isInView(const Pose& pose, const math::Vec2& worldPoint) const {
    const double eps = 1e-6;
    math::Vec2 local = math::worldToLocal(worldPoint - pose.position, pose.headingDeg);
    if (local.x <= eps || local.x > range_ + eps) return false;
    return std::fabs(local.y) <= (baseLength_ / 2.0) * (local.x / range_) + eps;
}

std::vector<Cell> Sensor::visibleCells(const Field& field, const Pose& pose) const {
    std::vector<Cell> cells;
    for (int row = 0; row < Field::kRows; ++row)
        for (int col = 0; col < Field::kCols; ++col) {
            Cell c{col, row};
            if (isInView(pose, field.cellCenter(c))) cells.push_back(c);
        }
    return cells;
}

Perception Sensor::capture(const Field& field, const Pose& pose, const Ball& ball) const {
    Perception p;
    p.visibleCells = visibleCells(field, pose);

    // Kamera hanya punya resolusi petak: bola dilaporkan di tengah petaknya.
    math::Vec2 ballCenter = field.cellCenter(field.cellOf(ball.getPosition()));
    if (isInView(pose, ballCenter)) {
        math::Vec2 rel = math::worldToLocal(ballCenter - pose.position, pose.headingDeg);
        p.ball.visible = true;
        p.ball.relative = rel;
        p.ball.distance = rel.length();
        p.ball.bearingDeg = math::normalizeAngle(math::toDegrees(std::atan2(rel.y, rel.x)));
    }
    return p;
}
