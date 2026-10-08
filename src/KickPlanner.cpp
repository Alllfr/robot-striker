#include "KickPlanner.hpp"

#include <algorithm>

#include "Ball.hpp"
#include "Navigator.hpp"

KickPlanner::KickPlanner(const Field& field)
    : field_(field),
      candidates_(Field::kCols * Field::kRows),
      kicks_(Field::kCols * Field::kRows, kUnreachable) {
    const double headings[4] = {0.0, 90.0, 180.0, -90.0};
    const KickDirection kicks[3] = {KickDirection::LEFT_DIAGONAL, KickDirection::STRAIGHT,
                                    KickDirection::RIGHT_DIAGONAL};

    // 1) Semua kombinasi (posisi berdiri, tendangan) untuk tiap petak bola.
    for (int row = 0; row < Field::kRows; ++row)
        for (int col = 0; col < Field::kCols; ++col) {
            Cell ball{col, row};
            math::Vec2 center = field_.cellCenter(ball);
            for (double h : headings) {
                // bola harus di petak depan robot -> robot berdiri satu petak "di belakang" bola
                Cell stand = Navigator::neighbor(ball, h + 180.0);
                if (!field_.containsCell(stand)) continue;
                for (KickDirection k : kicks) {
                    math::Vec2 dir = math::fromAngle(h + kickAngleOffset(k));
                    KickPrediction p = Ball::predictKick(field_, center, dir);
                    Candidate c{stand, h, k, p.goal, p.goal ? ball : field_.cellOf(p.finalPosition)};
                    if (!c.scores && c.end == ball) continue;  // tendangan tanpa kemajuan
                    candidates_[index(ball)].push_back(c);
                }
            }
        }

    // 2) DP: kicks_[c] = jumlah tendangan minimum dari petak c sampai gol.
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < static_cast<int>(kicks_.size()); ++i) {
            int best = kicks_[i];
            for (const Candidate& c : candidates_[i]) {
                int v = c.scores ? 1 : (kicks_[index(c.end)] < kUnreachable ? 1 + kicks_[index(c.end)] : kUnreachable);
                best = std::min(best, v);
            }
            if (best < kicks_[i]) {
                kicks_[i] = best;
                changed = true;
            }
        }
    }
}

KickPlan KickPlanner::plan(const Cell& ball, const Cell& robotCell, double robotHeadingDeg) const {
    KickPlan best;
    if (!field_.containsCell(ball) || !field_.containsCell(robotCell)) return best;
    int k = kicks_[index(ball)];
    if (k >= kUnreachable) return best;

    int bestTicks = kUnreachable;
    for (const Candidate& c : candidates_[index(ball)]) {
        bool onOptimalChain = c.scores || kicks_[index(c.end)] == k - 1;
        if (!onOptimalChain) continue;
        auto path = Navigator::findPath(field_, robotCell, c.stand, ball);
        if (!path) continue;
        int ticks = Navigator::travelTicks(robotCell, robotHeadingDeg, *path, c.heading);
        if (ticks < bestTicks) {
            bestTicks = ticks;
            best = {true, ball, c.stand, c.heading, c.kick, c.scores, c.end, ticks};
        }
    }
    return best;
}
