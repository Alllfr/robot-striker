#pragma once
#include <memory>
#include <string>
#include <vector>

#include "KickPlanner.hpp"
#include "Robot.hpp"
#include "StrikerState.hpp"

// Ingatan robot tentang bola, dibangun HANYA dari data kamera (+ prediksi tendangan sendiri).
struct BallMemory {
    bool known = false;
    Cell cell{};
};

// Data "otak" yang dibaca/ditulis oleh state-state.
struct StrikerBrain {
    BallMemory ball;
    KickPlan plan;
    std::vector<Cell> waypoints;   // titik-titik patroli pencarian
    size_t waypointIndex = 0;
    int scanTurns = 0;
    bool gaveUp = false;
};

// Striker: mewarisi Robot, meng-override think() dengan mendelegasikan ke State aktif.
//   SearchState -> ApproachState -> AlignState -> KickState -> (ulang)
class Striker : public Robot {
public:
    static constexpr int kMaxTransitionsPerTick = 6;

    Striker(const Field& field, const math::Vec2& startPosition, double headingDeg);
    ~Striker() override;

    Action think() override;
    void onActionRejected(const InvalidActionException& e) override;
    std::string stateName() const override;
    bool hasGivenUp() const override { return brain_.gaveUp; }

    // ---- API untuk State ----
    StrikerBrain& brain() { return brain_; }
    const KickPlanner& planner() const { return planner_; }
    void requestState(std::unique_ptr<StrikerState> next);   // berlaku setelah update() selesai
    bool isBallInFrontCell(const BallObservation& obs) const;

private:
    void updateMemory();
    void applyPendingState();
    void buildWaypoints();

    KickPlanner planner_;
    StrikerBrain brain_;
    std::unique_ptr<StrikerState> state_;
    std::unique_ptr<StrikerState> pending_;
};
