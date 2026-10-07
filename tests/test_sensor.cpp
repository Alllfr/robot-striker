#include "Sensor.hpp"
#include "test_framework.hpp"

namespace {
Pose centerPose(const Field& f, double heading) { return {f.cellCenter({9, 6}), heading}; }
Ball ballAtOffset(const Field& f, int dc, int dr) { return Ball(f.cellCenter({9 + dc, 6 + dr})); }
}  // namespace

TEST(camera_sees_15_cells_in_every_cardinal_heading) {
    Field f;
    Sensor s;
    for (double h : {0.0, 90.0, 180.0, -90.0})
        CHECK(s.visibleCells(f, centerPose(f, h)).size() == 15);  // 3 + 5 + 7
}

TEST(camera_triangle_shape_heading_east) {
    Field f;
    Sensor s;
    Pose p = centerPose(f, 0);
    CHECK(s.capture(f, p, ballAtOffset(f, 1, 0)).ball.visible);
    CHECK(s.capture(f, p, ballAtOffset(f, 1, 1)).ball.visible);
    CHECK(!s.capture(f, p, ballAtOffset(f, 1, 2)).ball.visible);   // di luar segitiga
    CHECK(s.capture(f, p, ballAtOffset(f, 3, 3)).ball.visible);    // ujung alas
    CHECK(!s.capture(f, p, ballAtOffset(f, 3, 4)).ball.visible);
    CHECK(!s.capture(f, p, ballAtOffset(f, 4, 0)).ball.visible);   // terlalu jauh (>1.5 m)
    CHECK(!s.capture(f, p, ballAtOffset(f, -1, 0)).ball.visible);  // di belakang
}

TEST(observation_is_relative_to_robot) {
    Field f;
    Sensor s;
    Perception p = s.capture(f, centerPose(f, 0), ballAtOffset(f, 2, 1));
    CHECK(p.ball.visible);
    CHECK_NEAR(p.ball.relative.x, 1.0, 1e-9);   // 2 petak depan
    CHECK_NEAR(p.ball.relative.y, 0.5, 1e-9);   // 1 petak kiri
    CHECK_NEAR(p.ball.distance, std::sqrt(1.25), 1e-9);
    CHECK(p.ball.bearingDeg > 0);               // kiri = bearing positif
}

TEST(observation_rotates_with_heading) {
    Field f;
    Sensor s;
    // robot menghadap utara (+y), bola 1 petak di utara -> tepat di depan
    Perception p = s.capture(f, centerPose(f, 90), ballAtOffset(f, 0, 1));
    CHECK(p.ball.visible);
    CHECK_NEAR(p.ball.relative.x, 0.5, 1e-9);
    CHECK_NEAR(p.ball.relative.y, 0.0, 1e-9);
}

TEST(camera_is_clipped_by_field_edges) {
    Field f;
    Sensor s;
    Pose corner{f.cellCenter({0, 0}), 180};  // menghadap dinding barat
    CHECK(s.visibleCells(f, corner).empty());
}
