#include "Navigator.hpp"

#include <cmath>
#include <queue>

#include "MathUtils.hpp"

Cell Navigator::neighbor(const Cell& from, double headingDeg) {
    double r = math::toRadians(headingDeg);
    return {from.col + static_cast<int>(std::lround(std::cos(r))),
            from.row + static_cast<int>(std::lround(std::sin(r)))};
}

std::optional<std::vector<Cell>> Navigator::findPath(const Field& field, const Cell& from, const Cell& to,
                                                     const std::optional<Cell>& blocked) {
    if (!field.containsCell(from) || !field.containsCell(to)) return std::nullopt;
    if (blocked && *blocked == to) return std::nullopt;
    if (from == to) return std::vector<Cell>{};

    auto idx = [](const Cell& c) { return c.row * Field::kCols + c.col; };
    std::vector<int> parent(Field::kCols * Field::kRows, -2);  // -2 = belum dikunjungi
    std::queue<Cell> q;
    parent[idx(from)] = -1;
    q.push(from);

    const double headings[4] = {0.0, 90.0, 180.0, -90.0};
    while (!q.empty()) {
        Cell cur = q.front();
        q.pop();
        if (cur == to) break;
        for (double h : headings) {
            Cell nb = neighbor(cur, h);
            if (!field.containsCell(nb) || parent[idx(nb)] != -2) continue;
            if (blocked && *blocked == nb) continue;
            parent[idx(nb)] = idx(cur);
            q.push(nb);
        }
    }
    if (parent[idx(to)] == -2) return std::nullopt;

    std::vector<Cell> path;
    for (int i = idx(to); i != idx(from); i = parent[i]) path.push_back({i % Field::kCols, i / Field::kCols});
    return std::vector<Cell>(path.rbegin(), path.rend());
}

int Navigator::travelTicks(const Cell& start, double startHeading, const std::vector<Cell>& path,
                           double finalHeading) {
    double heading = startHeading;
    Cell cur = start;
    int ticks = 0;
    for (const Cell& next : path) {
        double desired = math::toDegrees(std::atan2(next.row - cur.row, next.col - cur.col));
        ticks += static_cast<int>(std::lround(std::fabs(math::normalizeAngle(desired - heading)) / 90.0)) + 1;
        heading = desired;
        cur = next;
    }
    ticks += static_cast<int>(std::lround(std::fabs(math::normalizeAngle(finalHeading - heading)) / 90.0));
    return ticks;
}
