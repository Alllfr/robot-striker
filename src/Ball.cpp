#include "Ball.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

Ball::Ball(const math::Vec2& position) : position_(position) {}

void Ball::kick(const math::Vec2& direction) {
    if (direction.length() < 1e-9) throw std::invalid_argument("Arah tendangan nol");
    direction_ = direction.normalized();
    speed_ = kKickSpeed;
}

// Maju sejauh `dist` dari `from`; berhenti di batas lapangan pertama yang dilewati.
// Jika batas yang kena adalah garis gawang dan |y| <= setengah lebar gawang -> GOL.
Ball::Step Ball::advance(const Field& field, const math::Vec2& from, const math::Vec2& dir, double dist) {
    const double inf = std::numeric_limits<double>::infinity();
    const double eps = 1e-12;
    double tx = inf, ty = inf;
    if (dir.x > eps) tx = (field.maxX() - from.x) / dir.x;
    else if (dir.x < -eps) tx = (field.minX() - from.x) / dir.x;
    if (dir.y > eps) ty = (field.maxY() - from.y) / dir.y;
    else if (dir.y < -eps) ty = (field.minY() - from.y) / dir.y;

    double tWall = std::fmin(tx, ty);
    Step s;
    if (tWall <= dist) {
        s.position = from + dir * tWall;
        s.hitWall = true;
        s.goal = (tx <= ty) && dir.x > 0.0 && std::fabs(s.position.y) <= field.goalHalfWidth() + 1e-9;
    } else {
        s.position = from + dir * dist;
    }
    return s;
}

void Ball::update(const Field& field) {
    if (speed_ <= 0.0) return;
    Step s = advance(field, position_, direction_, speed_);
    position_ = s.position;
    speed_ -= kDeceleration;
    if (s.goal) {
        inGoal_ = true;
        speed_ = 0.0;
    } else if (s.hitWall) {
        // Keluar lapangan -> respawn di tengah lapangan, diam.
        outOfBounds_ = true;
        speed_ = 0.0;
        position_ = field.cellCenter(field.centerCell());
        return;
    }
    if (speed_ <= 1e-9) {
        speed_ = 0.0;
        // Bola yang berhenti di-snap ke tengah petak (grid ASCII).
        if (!inGoal_) position_ = field.cellCenter(field.cellOf(position_));
    }
}

KickPrediction Ball::predictKick(const Field& field, const math::Vec2& start, const math::Vec2& direction) {
    Ball sim(start);
    sim.kick(direction);
    KickPrediction result;
    while (sim.isMoving()) {
        sim.update(field);
        ++result.ticks;
    }
    result.finalPosition = sim.position_;
    result.goal = sim.inGoal_;
    result.outOfBounds = sim.outOfBounds_;
    return result;
}