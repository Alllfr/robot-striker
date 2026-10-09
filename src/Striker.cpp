#include "Striker.hpp"

#include <algorithm>
#include <cmath>

#include "Navigator.hpp"
#include "StrikerStates.hpp"

Striker::Striker(const Field& field, const math::Vec2& startPosition, double headingDeg)
    : Robot(field, startPosition, headingDeg),
      planner_(field),
      state_(std::make_unique<SearchState>()) {
    buildWaypoints();
}

Striker::~Striker() = default;

// Kamera melihat kotak 7x7 petak di sekeliling robot jika robot memutar 4 arah,
// jadi waypoint dengan jarak 7 petak sudah menutup seluruh lapangan 18x12.
void Striker::buildWaypoints() {
    std::vector<Cell> lattice;
    for (int row : {3, 9})
        for (int col : {3, 10, 15}) lattice.push_back({col, row});

    Cell current = getCell();
    brain_.waypoints.push_back(current);  // mulai dengan scan di tempat
    while (!lattice.empty()) {  // urutan: tetangga terdekat dulu
        auto it = std::min_element(lattice.begin(), lattice.end(), [&](const Cell& a, const Cell& b) {
            return std::abs(a.col - current.col) + std::abs(a.row - current.row) <
                   std::abs(b.col - current.col) + std::abs(b.row - current.row);
        });
        current = *it;
        if (current != brain_.waypoints.front()) brain_.waypoints.push_back(current);
        lattice.erase(it);
    }
}

void Striker::requestState(std::unique_ptr<StrikerState> next) { pending_ = std::move(next); }

void Striker::applyPendingState() {
    if (pending_) state_ = std::move(pending_);
}

std::string Striker::stateName() const { return state_->name(); }

bool Striker::isBallInFrontCell(const BallObservation& obs) const {
    return obs.visible && math::nearlyEqual(obs.relative.x, Field::kCellSize) &&
           math::nearlyEqual(obs.relative.y, 0.0);
}

// Memori bola hanya dari kamera. Jika petak yang diingat sedang terlihat tapi kosong -> lupakan.
void Striker::updateMemory() {
    const Perception& p = getPerception();
    if (p.ball.visible) {
        math::Vec2 world = getPosition() + math::localToWorld(p.ball.relative, getHeading());
        brain_.ball = {true, getField().cellOf(world)};
    } else if (brain_.ball.known) {
        for (const Cell& c : p.visibleCells)
            if (c == brain_.ball.cell) {
                brain_.ball.known = false;
                break;
            }
    }
}

Action Striker::think() {
    applyPendingState();
    updateMemory();
    for (int i = 0; i < kMaxTransitionsPerTick; ++i) {
        std::optional<Action> action = state_->update(*this);
        applyPendingState();
        if (action) return *action;
    }
    return Action::wait();
}

void Striker::onActionRejected(const InvalidActionException&) {
    brain_.plan.valid = false;                       // rencana lama tidak bisa dipercaya
    requestState(std::make_unique<SearchState>());   // mulai lagi dari pencarian
}
