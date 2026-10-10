#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#include "ConfigLoader.hpp"
#include "Exceptions.hpp"
#include "Simulator.hpp"

static void printUsage(const char* prog) {
    std::cout << "Pemakaian: " << prog << " [config.txt] [--default] [--animate <ms>] [--quiet]\n"
              << "  (tanpa argumen)  minta input koordinat Striker & bola dari keyboard\n"
              << "  config.txt       baca skenario dari file\n"
              << "  --default        pakai skenario bawaan tanpa bertanya\n"
              << "  --animate ms     bersihkan layar & beri jeda ms per tick\n"
              << "  --quiet          hanya cetak frame akhir\n";
}

// Baca satu angka dari keyboard; ulangi sampai valid.
static double askNumber(const std::string& label, double lo, double hi) {
    while (true) {
        std::cout << label << " [" << lo << " .. " << hi << "]: " << std::flush;
        double v;
        if (!(std::cin >> v)) {
            if (std::cin.eof()) throw ConfigException("Input berakhir sebelum semua koordinat diisi");
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Input bukan angka, coba lagi.\n";
            continue;
        }
        if (v < lo || v > hi) {
            std::cout << "  Di luar rentang, coba lagi.\n";
            continue;
        }
        return v;
    }
}

static SimulationConfig askConfig() {
    Field f;
    SimulationConfig cfg;
    std::cout << "== Input skenario (meter; pusat lapangan = (0,0)) ==\n";
    while (true) {
        cfg.strikerPosition.x = askNumber("Robot x", f.minX(), f.maxX());
        cfg.strikerPosition.y = askNumber("Robot y", f.minY(), f.maxY());
        double h;
        while (true) {
            h = askNumber("Arah hadap robot (0=timur/gawang, 90=utara, 180=barat, -90=selatan)", -180, 360);
            double q = h / 90.0;
            if (q == static_cast<int>(q)) break;
            std::cout << "  Heading harus kelipatan 90.\n";
        }
        cfg.strikerHeading = h;
        cfg.ballPosition.x = askNumber("Bola x", f.minX(), f.maxX());
        cfg.ballPosition.y = askNumber("Bola y", f.minY(), f.maxY());
        if (f.cellOf(cfg.strikerPosition) == f.cellOf(cfg.ballPosition)) {
            std::cout << "  Robot dan bola tidak boleh di petak yang sama, ulangi.\n";
            continue;
        }
        return cfg;
    }
}

int main(int argc, char** argv) {
    std::string configPath;
    bool useDefault = false;
    RunOptions opt;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help" || a == "-h") { printUsage(argv[0]); return 0; }
        else if (a == "--quiet") opt.quiet = true;
        else if (a == "--default") useDefault = true;
        else if (a == "--animate" && i + 1 < argc) { opt.delayMs = std::atoi(argv[++i]); opt.clearScreen = true; }
        else if (a.rfind("--", 0) == 0) { std::cerr << "Opsi tidak dikenal: " << a << "\n"; printUsage(argv[0]); return 2; }
        else configPath = a;
    }

    try {
        SimulationConfig cfg;
        if (!configPath.empty()) cfg = ConfigLoader::loadFromFile(configPath);
        else if (!useDefault) cfg = askConfig();
        Simulator sim(cfg);
        Outcome result = sim.run(std::cout, opt);
        return result == Outcome::GOAL ? 0 : 1;
    } catch (const ConfigException& e) {
        std::cerr << "Konfigurasi salah: " << e.what() << "\n";
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}