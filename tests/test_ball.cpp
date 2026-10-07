#include <stdexcept>

#include "Ball.hpp"
#include "test_framework.hpp"

TEST(ball_speed_profile_3_2_1) {
    Field f;
    Ball b({-4.25, 0.25});
    b.kick({1, 0});
    b.update(f);
    CHECK_NEAR(b.getPosition().x, -1.25, 1e-9);  // +3 m
    CHECK_NEAR(b.getSpeed(), 2.0, 1e-9);
    b.update(f);
    CHECK_NEAR(b.getPosition().x, 0.75, 1e-9);   // +2 m
    b.update(f);
    CHECK(!b.isMoving());                        // +1 m lalu berhenti
    CHECK_NEAR(b.getPosition().x, 1.75, 1e-9);   // 1.75 sudah tengah petak
}

TEST(ball_scores_when_crossing_goal_line_inside_goal) {
    Field f;
    Ball b({0.25, 0.25});
    b.kick({1, 0});
    b.update(f);
    b.update(f);
    CHECK(b.isInGoal());
    CHECK(!b.isMoving());
}

TEST(ball_misses_goal_when_outside_posts) {
    Field f;
    Ball b({0.25, 2.25});
    b.kick({1, 0});
    for (int i = 0; i < 4; ++i) b.update(f);
    CHECK(!b.isInGoal());
    CHECK_NEAR(b.getPosition().x, 4.25, 1e-9);  // berhenti di petak terakhir
}

TEST(ball_stops_at_side_wall) {
    Field f;
    Ball b({0.25, 2.25});
    b.kick({0, 1});
    b.update(f);
    CHECK(!b.isMoving());
    CHECK_NEAR(b.getPosition().y, 2.75, 1e-9);
}

TEST(predictKick_matches_real_simulation) {
    Field f;
    math::Vec2 start = f.cellCenter({5, 4});
    for (double angle : {0.0, 45.0, -45.0, 90.0, 135.0, 180.0, -135.0, -90.0}) {
        math::Vec2 dir = math::fromAngle(angle);
        KickPrediction p = Ball::predictKick(f, start, dir);
        Ball real(start);
        real.kick(dir);
        while (real.isMoving()) real.update(f);
        CHECK_NEAR(p.finalPosition.x, real.getPosition().x, 1e-12);
        CHECK_NEAR(p.finalPosition.y, real.getPosition().y, 1e-12);
        CHECK(p.goal == real.isInGoal());
    }
}

TEST(diagonal_kick_can_score) {
    Field f;
    // dari (2.25, 2.25) tembak diagonal kanan-bawah: lintasan memotong x=4.5 di y=0 -> gol
    KickPrediction p = Ball::predictKick(f, {2.25, 2.25}, math::fromAngle(-45));
    CHECK(p.goal);
}

TEST(zero_direction_is_rejected) {
    Ball b;
    CHECK_THROWS(b.kick({0, 0}), std::invalid_argument);
}
