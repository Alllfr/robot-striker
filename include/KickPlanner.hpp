#pragma once
#include <vector>

#include "Action.hpp"
#include "Field.hpp"
#include "MathUtils.hpp"

// Rencana tendangan: berdiri di mana, menghadap ke mana, menendang ke arah mana.
struct KickPlan {
    bool valid = false;
    Cell ballCell{};
    Cell standCell{};
    double heading = 0.0;
    KickDirection kick = KickDirection::STRAIGHT;
    bool scores = false;      // tendangan ini langsung menghasilkan gol?
    Cell predictedEnd{};      // petak bola berhenti jika tidak gol
    int estimatedTicks = 0;   // jalan + putar menuju posisi tembak
};

// Planner murni (tanpa state robot). Saat dibuat, ia menghitung untuk SETIAP petak bola
// berapa tendangan minimum menuju gol (DP). plan() lalu memilih posisi tembak
// terdekat yang mengikuti rantai tendangan optimal -> dijamin ada kemajuan.
class KickPlanner {
public:
    static constexpr int kUnreachable = 1000000;

    explicit KickPlanner(const Field& field);

    KickPlan plan(const Cell& ball, const Cell& robotCell, double robotHeadingDeg) const;
    int kicksToGoal(const Cell& ball) const { return kicks_[index(ball)]; }

private:
    struct Candidate {
        Cell stand;
        double heading;
        KickDirection kick;
        bool scores;
        Cell end;
    };

    static int index(const Cell& c) { return c.row * Field::kCols + c.col; }

    const Field& field_;
    std::vector<std::vector<Candidate>> candidates_;
    std::vector<int> kicks_;
};
