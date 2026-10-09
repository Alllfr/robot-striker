#pragma once
#include "Striker.hpp"

// SEARCH_BALL: patroli antar waypoint, di tiap waypoint putar 360 derajat memindai.
class SearchState : public StrikerState {
public:
    std::optional<Action> update(Striker& s) override;
    const char* name() const override { return "SearchState"; }
};

// APPROACH_BALL: berjalan ke posisi tembak (di belakang bola, menghindari petak bola).
class ApproachState : public StrikerState {
public:
    std::optional<Action> update(Striker& s) override;
    const char* name() const override { return "ApproachState"; }
};

// ALIGN_TO_GOAL: putar ke heading tembak, pastikan bola persis di petak depan.
class AlignState : public StrikerState {
public:
    std::optional<Action> update(Striker& s) override;
    const char* name() const override { return "AlignState"; }
};

// KICK: tendang, lalu tunggu bola berhenti (3 m + 2 m + 1 m = 3 tick).
class KickState : public StrikerState {
public:
    static constexpr int kSettleTicks = 2;
    std::optional<Action> update(Striker& s) override;
    const char* name() const override { return "KickState"; }

private:
    bool kicked_ = false;
    int settle_ = 0;
};
