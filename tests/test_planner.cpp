#include "KickPlanner.hpp"
#include "test_framework.hpp"

TEST(planner_center_ball_needs_one_kick) {
    Field f;
    KickPlanner p(f);
    CHECK(p.kicksToGoal({9, 6}) == 1);
}

TEST(planner_picks_straight_kick_when_already_in_position) {
    Field f;
    KickPlanner p(f);
    // bola di (9,6); robot di (8,6) menghadap timur -> tendang lurus, tanpa jalan
    KickPlan plan = p.plan({9, 6}, {8, 6}, 0);
    CHECK(plan.valid);
    CHECK(plan.scores);
    CHECK(plan.standCell == Cell({8, 6}));
    CHECK(plan.estimatedTicks == 0);
    CHECK(plan.kick == KickDirection::STRAIGHT);
}

TEST(planner_far_ball_takes_multiple_kicks) {
    Field f;
    KickPlanner p(f);
    // bola di dekat gawang sendiri: 6 m tidak cukup untuk sampai gawang
    CHECK(p.kicksToGoal({2, 6}) >= 2);
}

TEST(planner_chain_always_makes_progress) {
    Field f;
    KickPlanner p(f);
    for (int row = 0; row < Field::kRows; ++row)
        for (int col = 0; col < Field::kCols; ++col) {
            Cell ball{col, row};
            int k = p.kicksToGoal(ball);
            if (k >= KickPlanner::kUnreachable) continue;
            Cell robot{(col + 5) % Field::kCols, (row + 3) % Field::kRows};
            if (robot == ball) continue;
            KickPlan plan = p.plan(ball, robot, 0);
            CHECK(plan.valid);
            if (!plan.scores) CHECK(p.kicksToGoal(plan.predictedEnd) == k - 1);
        }
}

TEST(planner_interior_cells_are_all_solvable) {
    Field f;
    KickPlanner p(f);
    for (int row = 1; row < Field::kRows - 1; ++row)
        for (int col = 1; col < Field::kCols - 1; ++col)
            CHECK(p.kicksToGoal({col, row}) < KickPlanner::kUnreachable);
}

TEST(planner_corner_is_unsolvable_and_plan_is_invalid) {
    Field f;
    KickPlanner p(f);
    Cell corner{17, 11};
    CHECK(p.kicksToGoal(corner) >= KickPlanner::kUnreachable);
    CHECK(!p.plan(corner, {5, 5}, 0).valid);
}
