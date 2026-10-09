#include <sstream>

#include "Simulator.hpp"
#include "test_framework.hpp"

namespace {
SimulationConfig makeConfig(Cell robot, double heading, Cell ball) {
    Field f;
    SimulationConfig c;
    c.strikerPosition = f.cellCenter(robot);
    c.strikerHeading = heading;
    c.ballPosition = f.cellCenter(ball);
    return c;
}
}  // namespace

TEST(simulator_rejects_speed_over_limit) {
    Simulator sim(makeConfig({5, 5}, 0, {10, 8}));
    CHECK_THROWS(sim.executeAction(Action::moveForward(1.0)), SpeedLimitException);
}

TEST(simulator_rejects_big_turn_and_odd_angles) {
    Simulator sim(makeConfig({5, 5}, 0, {10, 8}));
    CHECK_THROWS(sim.executeAction(Action::turn(180)), TurnLimitException);
    CHECK_THROWS(sim.executeAction(Action::turn(45)), InvalidActionException);
}

TEST(simulator_rejects_leaving_field) {
    Simulator sim(makeConfig({0, 5}, 180, {10, 8}));
    CHECK_THROWS(sim.executeAction(Action::moveForward()), OutOfBoundsException);
}

TEST(simulator_rejects_walking_into_ball) {
    Simulator sim(makeConfig({5, 5}, 0, {6, 5}));
    CHECK_THROWS(sim.executeAction(Action::moveForward()), BlockedException);
}

TEST(simulator_rejects_kick_when_ball_not_in_front) {
    Simulator sim(makeConfig({5, 5}, 0, {10, 8}));
    CHECK_THROWS(sim.executeAction(Action::kick(KickDirection::STRAIGHT)), KickNotPossibleException);
}

TEST(simulator_valid_actions_update_robot) {
    Simulator sim(makeConfig({5, 5}, 0, {10, 8}));
    sim.executeAction(Action::moveForward());
    CHECK(sim.robot().getCell() == Cell({6, 5}));
    sim.executeAction(Action::turn(90));
    CHECK_NEAR(sim.robot().getHeading(), 90.0, 1e-9);
}

TEST(simulator_rejects_bad_start_positions) {
    SimulationConfig c;
    c.ballPosition = {10.0, 0.0};
    CHECK_THROWS(Simulator{c}, ConfigException);
    CHECK_THROWS(Simulator{makeConfig({5, 5}, 0, {5, 5})}, ConfigException);
}

TEST(striker_scores_default_scenario) {
    Simulator sim{SimulationConfig{}};
    std::ostringstream sink;
    CHECK(sim.run(sink, RunOptions{0, false, true}) == Outcome::GOAL);
}

TEST(striker_scores_from_every_interior_ball_cell) {
    for (int row = 1; row < Field::kRows - 1; ++row)
        for (int col = 1; col < Field::kCols - 1; ++col) {
            Cell robot = (col < 9) ? Cell{16, 1} : Cell{1, 10};
            Simulator sim(makeConfig(robot, 90, {col, row}));
            while (sim.outcome() == Outcome::RUNNING) sim.step();
            CHECK(sim.outcome() == Outcome::GOAL);
        }
}

TEST(striker_gives_up_on_trapped_ball) {
    Simulator sim(makeConfig({5, 5}, 0, {17, 11}));
    while (sim.outcome() == Outcome::RUNNING) sim.step();
    CHECK(sim.outcome() == Outcome::GAVE_UP);
}

TEST(robot_never_reads_ball_directly) {
    // Bola di belakang robot & di luar kamera: robot tidak boleh "tahu" -> tetap mencari dulu.
    Simulator sim(makeConfig({10, 6}, 0, {2, 6}));
    sim.step();
    CHECK(sim.robot().stateName() == "SearchState");
}
