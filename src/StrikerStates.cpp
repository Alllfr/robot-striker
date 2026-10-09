#include "StrikerStates.hpp"

#include <cmath>

#include "Navigator.hpp"

namespace {
double turnToward(double diffDeg) { return diffDeg > 0 ? Robot::kMaxTurn : -Robot::kMaxTurn; }
}  // namespace

// ---------------------------------------------------------------- SearchState
std::optional<Action> SearchState::update(Striker& s) {
    StrikerBrain& b = s.brain();
    if (b.ball.known) {
        s.requestState(std::make_unique<ApproachState>());
        return std::nullopt;
    }

    Cell target = b.waypoints[b.waypointIndex];
    if (s.getCell() != target) {
        auto path = Navigator::findPath(s.getField(), s.getCell(), target);
        if (!path || path->empty()) {
            b.waypointIndex = (b.waypointIndex + 1) % b.waypoints.size();
            return Action::wait();
        }
        return s.stepToward(path->front());
    }

    // Di waypoint: putar 3x90 derajat -> 4 arah pandang sudah terlihat.
    if (b.scanTurns < 3) {
        ++b.scanTurns;
        return Action::turn(Robot::kMaxTurn);
    }
    b.scanTurns = 0;
    b.waypointIndex = (b.waypointIndex + 1) % b.waypoints.size();
    return std::nullopt;  // ulangi update() menuju waypoint berikutnya
}

// ------------------------------------------------------------- ApproachState
std::optional<Action> ApproachState::update(Striker& s) {
    StrikerBrain& b = s.brain();
    if (!b.ball.known) {
        s.requestState(std::make_unique<SearchState>());
        return std::nullopt;
    }

    if (!b.plan.valid || b.plan.ballCell != b.ball.cell) {
        b.plan = s.planner().plan(b.ball.cell, s.getCell(), s.getHeading());
        if (!b.plan.valid) {  // mis. bola terjebak di pojok: tidak ada tendangan yang berguna
            b.gaveUp = true;
            return Action::wait();
        }
    }

    if (s.getCell() == b.plan.standCell) {
        s.requestState(std::make_unique<AlignState>());
        return std::nullopt;
    }

    auto path = Navigator::findPath(s.getField(), s.getCell(), b.plan.standCell, b.ball.cell);
    if (!path || path->empty()) {
        b.plan.valid = false;  // hitung ulang tick berikutnya
        return Action::wait();
    }
    return s.stepToward(path->front());
}

// ---------------------------------------------------------------- AlignState
std::optional<Action> AlignState::update(Striker& s) {
    StrikerBrain& b = s.brain();
    bool planStillValid = b.plan.valid && b.ball.known && b.ball.cell == b.plan.ballCell &&
                          s.getCell() == b.plan.standCell;
    if (!planStillValid) {
        s.requestState(std::make_unique<ApproachState>());
        return std::nullopt;
    }

    double diff = math::normalizeAngle(b.plan.heading - s.getHeading());
    if (std::fabs(diff) > 1e-6) return Action::turn(turnToward(diff));

    // Menghadap benar: verifikasi lewat kamera bahwa bola persis di petak depan.
    if (s.isBallInFrontCell(s.getPerception().ball)) {
        s.requestState(std::make_unique<KickState>());
        return std::nullopt;
    }
    s.requestState(std::make_unique<ApproachState>());
    return std::nullopt;
}

// ----------------------------------------------------------------- KickState
std::optional<Action> KickState::update(Striker& s) {
    StrikerBrain& b = s.brain();
    if (!kicked_) {
        kicked_ = true;
        settle_ = kSettleTicks;
        KickDirection dir = b.plan.kick;
        // Robot tahu fisika tendangannya sendiri -> ingat prediksi tempat bola berhenti.
        b.ball = {!b.plan.scores, b.plan.predictedEnd};
        b.plan.valid = false;
        return Action::kick(dir);
    }
    if (settle_ > 0) {
        --settle_;
        return Action::wait();
    }
    s.requestState(std::make_unique<SearchState>());
    return std::nullopt;
}
