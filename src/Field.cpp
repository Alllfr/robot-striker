#include "Field.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

bool Field::contains(const math::Vec2& p) const {
    return p.x >= minX() && p.x <= maxX() && p.y >= minY() && p.y <= maxY();
}

bool Field::containsCell(const Cell& c) const {
    return c.col >= 0 && c.col < kCols && c.row >= 0 && c.row < kRows;
}

Cell Field::cellOf(const math::Vec2& p) const {
    int col = static_cast<int>(std::floor((p.x - minX()) / kCellSize));
    int row = static_cast<int>(std::floor((p.y - minY()) / kCellSize));
    return {std::clamp(col, 0, kCols - 1), std::clamp(row, 0, kRows - 1)};
}

math::Vec2 Field::cellCenter(const Cell& c) const {
    if (!containsCell(c)) throw std::out_of_range("Cell di luar lapangan");
    return {minX() + (c.col + 0.5) * kCellSize, minY() + (c.row + 0.5) * kCellSize};
}

bool Field::isGoalRow(int row) const {
    double cy = minY() + (row + 0.5) * kCellSize;
    return std::fabs(cy) <= goalHalfWidth();
}

bool Field::isBoundaryCell(const Cell& c) const {
    return c.col == 0 || c.row == 0 || c.col == kCols - 1 || c.row == kRows - 1;
}
