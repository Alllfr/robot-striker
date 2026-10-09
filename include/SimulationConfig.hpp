#pragma once
#include "MathUtils.hpp"

// Skenario awal simulasi (posisi dalam meter, akan di-snap ke tengah petak).
struct SimulationConfig {
    math::Vec2 strikerPosition{-3.25, 1.75};
    double strikerHeading = 0.0;
    math::Vec2 ballPosition{-0.75, -1.25};
    int maxTicks = 300;
};
