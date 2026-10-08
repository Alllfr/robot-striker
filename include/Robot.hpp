#pragma once
#include <string>

#include "Action.hpp"
#include "Ball.hpp"
#include "Exceptions.hpp"
#include "Field.hpp"
#include "MathUtils.hpp"
#include "Sensor.hpp"

// Abstract class Robot.
//  - atribut private, diakses lewat getter/setter yang memvalidasi
//  - Robot HAS-A Sensor (composition)
//  - logika navigasi umum (jarak, bearing, langkah ke petak) ada di sini (DRY)
//  - subclass wajib meng-override think()
class Robot {
public:
    static constexpr double kMaxSpeed = 0.5;   // m/tick
    static constexpr double kMaxTurn = 90.0;   // derajat/tick

    Robot(const Field& field, const math::Vec2& startPosition, double headingDeg);
    virtual ~Robot() = default;
    Robot(const Robot&) = delete;
    Robot& operator=(const Robot&) = delete;

    // ---- Siklus Sense - Think - Act ----
    void sense(const Ball& ball);            // SENSE: hanya lewat Sensor (kamera)
    virtual Action think() = 0;              // THINK: diputuskan oleh subclass
    // ACT dilakukan Simulator; jika ditolak, robot diberi tahu lewat hook ini.
    virtual void onActionRejected(const InvalidActionException&) {}

    // ---- Info untuk tampilan ----
    virtual std::string stateName() const { return "-"; }
    virtual bool hasGivenUp() const { return false; }

    // ---- Getter / setter tervalidasi ----
    const math::Vec2& getPosition() const { return position_; }
    void setPosition(const math::Vec2& p);   // throw OutOfBoundsException
    double getHeading() const { return heading_; }
    void setHeading(double deg);             // harus kelipatan 90, dinormalisasi
    double getSpeed() const { return speed_; }
    void setSpeed(double s);                 // throw SpeedLimitException jika > 0.5 / < 0

    Pose getPose() const { return {position_, heading_}; }
    Cell getCell() const { return field_.cellOf(position_); }
    const Field& getField() const { return field_; }
    const Sensor& getSensor() const { return sensor_; }
    const Perception& getPerception() const { return perception_; }

    // ---- Navigasi umum (dipakai semua subclass) ----
    double distanceTo(const math::Vec2& target) const;   // meter
    double bearingTo(const math::Vec2& target) const;    // relatif ke heading, -180..180
    math::Vec2 forwardUnit() const;                      // vektor satuan arah hadap
    Cell frontCell() const;                              // petak tepat di depan robot
    Action stepToward(const Cell& adjacentCell) const;   // putar dulu, lalu maju 1 petak

private:
    const Field& field_;
    math::Vec2 position_;
    double heading_ = 0.0;
    double speed_ = 0.0;
    Sensor sensor_;
    Perception perception_;
};
