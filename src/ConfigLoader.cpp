#include "ConfigLoader.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

#include "Exceptions.hpp"
#include "Field.hpp"

namespace {
std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

double toNumber(const std::string& key, const std::string& value, int lineNo) {
    try {
        size_t used = 0;
        double v = std::stod(value, &used);
        if (used != value.size()) throw std::invalid_argument("sisa karakter");
        return v;
    } catch (const std::exception&) {
        throw ConfigException("Baris " + std::to_string(lineNo) + ": nilai '" + value + "' untuk '" + key +
                              "' bukan angka");
    }
}
}  // namespace

SimulationConfig ConfigLoader::parse(const std::string& text) {
    SimulationConfig cfg;
    std::istringstream in(text);
    std::string line;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos)
            throw ConfigException("Baris " + std::to_string(lineNo) + ": format harus kunci = nilai");
        std::string key = trim(line.substr(0, eq));
        double v = toNumber(key, trim(line.substr(eq + 1)), lineNo);

        if (key == "striker_x") cfg.strikerPosition.x = v;
        else if (key == "striker_y") cfg.strikerPosition.y = v;
        else if (key == "striker_heading") cfg.strikerHeading = v;
        else if (key == "ball_x") cfg.ballPosition.x = v;
        else if (key == "ball_y") cfg.ballPosition.y = v;
        else if (key == "max_ticks") cfg.maxTicks = static_cast<int>(v);
        else throw ConfigException("Baris " + std::to_string(lineNo) + ": kunci tidak dikenal '" + key + "'");
    }

    // Validasi
    Field field;
    if (!field.contains(cfg.strikerPosition)) throw ConfigException("Posisi Striker di luar lapangan");
    if (!field.contains(cfg.ballPosition)) throw ConfigException("Posisi bola di luar lapangan");
    double q = cfg.strikerHeading / 90.0;
    if (std::fabs(q - std::round(q)) > 1e-9) throw ConfigException("striker_heading harus kelipatan 90");
    if (cfg.maxTicks <= 0) throw ConfigException("max_ticks harus > 0");
    if (field.cellOf(cfg.strikerPosition) == field.cellOf(cfg.ballPosition))
        throw ConfigException("Striker dan bola tidak boleh di petak yang sama");
    return cfg;
}

SimulationConfig ConfigLoader::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) throw ConfigException("Tidak bisa membuka file konfigurasi: " + path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parse(buffer.str());
}
