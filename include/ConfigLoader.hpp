#pragma once
#include <string>

#include "SimulationConfig.hpp"

// Level 3 - File Konfigurasi. Format sederhana "kunci = nilai", baris diawali '#' = komentar.
// Kunci: striker_x, striker_y, striker_heading, ball_x, ball_y, max_ticks
class ConfigLoader {
public:
    static SimulationConfig loadFromFile(const std::string& path);   // throw ConfigException
    static SimulationConfig parse(const std::string& text);          // dipakai juga oleh unit test
};
