#pragma once
#include "MathUtils.hpp"

// Satu petak grid. col 0 = paling kiri (x = -4.5), row 0 = paling bawah (y = -3.0).
struct Cell {
    int col = 0;
    int row = 0;
    bool operator==(const Cell& o) const { return col == o.col && row == o.row; }
    bool operator!=(const Cell& o) const { return !(*this == o); }
};

// Lapangan 9 m x 6 m, pusat (0,0), 1 petak = 0.5 m  ->  18 x 12 petak.
// Gawang lawan ada di garis x = +4.5, lebar 3 m (|y| <= 1.5).
class Field {
public:
    static constexpr double kWidth = 9.0;
    static constexpr double kHeight = 6.0;
    static constexpr double kCellSize = 0.5;
    static constexpr double kGoalWidth = 3.0;
    static constexpr int kCols = 18;
    static constexpr int kRows = 12;

    double minX() const { return -kWidth / 2.0; }
    double maxX() const { return kWidth / 2.0; }
    double minY() const { return -kHeight / 2.0; }
    double maxY() const { return kHeight / 2.0; }

    double goalLineX() const { return maxX(); }
    double goalHalfWidth() const { return kGoalWidth / 2.0; }
    math::Vec2 goalCenter() const { return {goalLineX(), 0.0}; }

    bool contains(const math::Vec2& p) const;
    bool containsCell(const Cell& c) const;

    Cell cellOf(const math::Vec2& p) const;          // di-clamp ke dalam lapangan
    math::Vec2 cellCenter(const Cell& c) const;      // throw std::out_of_range jika petak invalid
    bool isGoalRow(int row) const;                   // untuk menggambar '#'
    bool isBoundaryCell(const Cell& c) const;
    Cell centerCell() const { return {kCols / 2, kRows / 2}; }   // petak titik tengah (respawn bola)
};
