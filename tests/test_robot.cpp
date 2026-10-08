#include "Navigator.hpp"
#include "Robot.hpp"
#include "test_framework.hpp"

namespace {
// Robot konkret minimal hanya untuk menguji class abstrak Robot.
class DummyRobot : public Robot {
public:
    using Robot::Robot;
    Action think() override { return Action::wait(); }
};
}  // namespace

TEST(robot_setters_validate_input) {
    Field f;
    DummyRobot r(f, {0.25, 0.25}, 0);
    CHECK_THROWS(r.setSpeed(0.6), SpeedLimitException);
    CHECK_THROWS(r.setSpeed(-0.1), InvalidActionException);
    CHECK_THROWS(r.setPosition({5.0, 0.0}), OutOfBoundsException);
    CHECK_THROWS(r.setHeading(45.0), InvalidActionException);
    r.setHeading(270.0);
    CHECK_NEAR(r.getHeading(), -90.0, 1e-9);  // dinormalisasi
}

TEST(robot_start_outside_field_throws) {
    Field f;
    CHECK_THROWS(DummyRobot(f, {9.0, 0.0}, 0), OutOfBoundsException);
}

TEST(robot_distance_and_bearing_use_math_helpers) {
    Field f;
    DummyRobot r(f, {0.25, 0.25}, 90);   // menghadap utara
    CHECK_NEAR(r.distanceTo({0.25, 1.25}), 1.0, 1e-9);
    CHECK_NEAR(r.bearingTo({0.25, 1.25}), 0.0, 1e-9);     // lurus di depan
    CHECK_NEAR(r.bearingTo({1.25, 0.25}), -90.0, 1e-9);   // timur = kanan
    CHECK_NEAR(r.bearingTo({-0.75, 0.25}), 90.0, 1e-9);   // barat = kiri
}

TEST(robot_front_cell_and_step_toward) {
    Field f;
    DummyRobot r(f, f.cellCenter({5, 5}), 0);
    CHECK(r.frontCell() == Cell({6, 5}));
    Action a = r.stepToward({6, 5});
    CHECK(a.type == ActionType::MOVE_FORWARD);
    Action b = r.stepToward({5, 6});  // utara = putar kiri dulu
    CHECK(b.type == ActionType::TURN && b.amount > 0);
    Action c = r.stepToward({4, 5});  // belakang = putar 90 dulu
    CHECK(c.type == ActionType::TURN);
    Action d = r.stepToward({8, 8});  // bukan tetangga
    CHECK(d.type == ActionType::WAIT);
}

TEST(sense_uses_camera_only) {
    Field f;
    DummyRobot r(f, f.cellCenter({5, 5}), 0);
    Ball visible(f.cellCenter({6, 5}));
    r.sense(visible);
    CHECK(r.getPerception().ball.visible);
    Ball hidden(f.cellCenter({0, 0}));
    r.sense(hidden);
    CHECK(!r.getPerception().ball.visible);
}

TEST(navigator_shortest_path_and_blocking) {
    Field f;
    auto p = Navigator::findPath(f, {0, 0}, {3, 0});
    CHECK(p && p->size() == 3);
    auto around = Navigator::findPath(f, {0, 5}, {2, 5}, Cell{1, 5});
    CHECK(around && around->size() == 4);   // memutar lewat baris lain
    for (const Cell& c : *around) CHECK(c != Cell({1, 5}));
    CHECK(Navigator::findPath(f, {0, 0}, {0, 0})->empty());
    CHECK(!Navigator::findPath(f, {0, 0}, {20, 0}));
}

TEST(navigator_travel_ticks_counts_turns) {
    // menghadap timur, jalan 2 petak timur lalu 1 utara, akhir menghadap timur
    int t = Navigator::travelTicks({0, 0}, 0, {{1, 0}, {2, 0}, {2, 1}}, 0);
    CHECK(t == 2 + (1 + 1) + 1);  // 2 maju + (putar+maju) + putar balik
}
