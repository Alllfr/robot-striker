#include "MathUtils.hpp"
#include "test_framework.hpp"

using namespace math;

TEST(normalizeAngle_wraps_into_range) {
    CHECK_NEAR(normalizeAngle(190.0), -170.0, 1e-9);
    CHECK_NEAR(normalizeAngle(-190.0), 170.0, 1e-9);
    CHECK_NEAR(normalizeAngle(540.0), 180.0, 1e-9);
    CHECK_NEAR(normalizeAngle(-180.0), 180.0, 1e-9);
    CHECK_NEAR(normalizeAngle(0.0), 0.0, 1e-9);
    CHECK_NEAR(normalizeAngle(360.0), 0.0, 1e-9);
}

TEST(distance_is_euclidean) {
    CHECK_NEAR(distance({0, 0}, {3, 4}), 5.0, 1e-9);
    CHECK_NEAR(distance({-1, -1}, {-1, -1}), 0.0, 1e-9);
}

TEST(bearing_matches_atan2) {
    CHECK_NEAR(bearingDeg({0, 0}, {1, 1}), 45.0, 1e-9);
    CHECK_NEAR(bearingDeg({0, 0}, {-1, 0}), 180.0, 1e-9);
    CHECK_NEAR(bearingDeg({1, 1}, {1, 0}), -90.0, 1e-9);
}

TEST(rotate_and_local_frame) {
    Vec2 r = rotate({1, 0}, 90);
    CHECK_NEAR(r.x, 0.0, 1e-9);
    CHECK_NEAR(r.y, 1.0, 1e-9);
    // robot menghadap +y (90 deg): titik (0,1) ada tepat di depan robot
    Vec2 local = worldToLocal({0, 1}, 90);
    CHECK_NEAR(local.x, 1.0, 1e-9);
    CHECK_NEAR(local.y, 0.0, 1e-9);
    Vec2 back = localToWorld(local, 90);
    CHECK_NEAR(back.x, 0.0, 1e-9);
    CHECK_NEAR(back.y, 1.0, 1e-9);
}

TEST(vec2_operations) {
    Vec2 a{1, 2}, b{3, -1};
    CHECK_NEAR((a + b).x, 4.0, 1e-9);
    CHECK_NEAR((a - b).y, 3.0, 1e-9);
    CHECK_NEAR(a.dot(b), 1.0, 1e-9);
    Vec2 up{0, 5}, zero{0, 0};
    CHECK_NEAR(up.normalized().y, 1.0, 1e-9);
    CHECK_NEAR(zero.normalized().length(), 0.0, 1e-9);
}
