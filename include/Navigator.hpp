#pragma once
#include <optional>
#include <vector>

#include "Field.hpp"

// Navigasi grid murni (tanpa state): BFS jalur terpendek 4-arah + estimasi biaya tick.
class Navigator {
public:
    // Petak satu langkah searah heading (kelipatan 90).
    static Cell neighbor(const Cell& from, double headingDeg);

    // Jalur terpendek dari `from` ke `to` (tanpa `from`, termasuk `to`).
    // nullopt jika tidak terjangkau. `blocked` = petak yang tidak boleh dilewati.
    static std::optional<std::vector<Cell>> findPath(const Field& field, const Cell& from, const Cell& to,
                                                     const std::optional<Cell>& blocked = std::nullopt);

    // Jumlah tick (putar 90 deg = 1 tick, maju 1 petak = 1 tick) untuk menempuh jalur
    // lalu berputar ke finalHeading.
    static int travelTicks(const Cell& start, double startHeading, const std::vector<Cell>& path,
                           double finalHeading);
};
