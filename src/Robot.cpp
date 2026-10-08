#include "Robot.hpp"

#include <cmath>

#include "Navigator.hpp"

Robot::Robot(const Field& field, const math::Vec2& startPosition, double headingDeg) : field_(field) {
    setPosition(field_.cellCenter(field_.cellOf(startPosition)));  // validasi di dalam setPosition
    if (!field_.contains(startPosition)) throw OutOfBoundsException("Posisi awal robot di luar lapangan");
    setHeading(headingDeg);
}

void Robot::sense(const Ball& ball) { perception_ = sensor_.capture(field_, getPose(), ball); }

void Robot::setPosition(const math::Vec2& p) {
    if (!field_.contains(p)) throw OutOfBoundsException("Posisi robot di luar lapangan");
    position_ = p;
}

void Robot::setHeading(double deg) {
    double q = deg / 90.0;
    if (std::fabs(q - std::round(q)) > 1e-9)
        throw InvalidActionException("Heading robot harus kelipatan 90 derajat (grid)");
    heading_ = math::normalizeAngle(std::round(q) * 90.0);
}

void Robot::setSpeed(double s) {
    if (s < 0.0) throw InvalidActionException("Kecepatan tidak boleh negatif");
    if (s > kMaxSpeed + 1e-9) throw SpeedLimitException("Kecepatan melebihi 0.5 m/tick");
    speed_ = s;
}

double Robot::distanceTo(const math::Vec2& target) const { return math::distance(position_, target); }

double Robot::bearingTo(const math::Vec2& target) const {
    return math::normalizeAngle(math::bearingDeg(position_, target) - heading_);
}

math::Vec2 Robot::forwardUnit() const { return math::fromAngle(heading_); }

Cell Robot::frontCell() const { return Navigator::neighbor(getCell(), heading_); }

Action Robot::stepToward(const Cell& target) const {
    Cell me = getCell();
    int dx = target.col - me.col, dy = target.row - me.row;
    if (std::abs(dx) + std::abs(dy) != 1) return Action::wait();  // bukan petak bersebelahan
    double desired = math::toDegrees(std::atan2(dy, dx));
    double diff = math::normalizeAngle(desired - heading_);
    if (std::fabs(diff) > 1e-6) return Action::turn(diff > 0 ? kMaxTurn : -kMaxTurn);
    return Action::moveForward(Field::kCellSize);
}
