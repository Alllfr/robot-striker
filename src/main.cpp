#include <cstdlib>
#include <iostream>
#include <string>

#include "ConfigLoader.hpp"
#include "Exceptions.hpp"
#include "Simulator.hpp"

static void printUsage(const char* prog) {
    std::cout << "Pemakaian: " << prog << " [config.txt] [--animate <ms>] [--quiet]\n"
              << "  config.txt     file skenario (opsional, default: skenario bawaan)\n"
              << "  --animate ms   bersihkan layar & beri jeda ms per tick\n"
              << "  --quiet        hanya cetak frame akhir\n";
}

int main(int argc, char** argv) {
    std::string configPath;
    RunOptions opt;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help" || a == "-h") { printUsage(argv[0]); return 0; }
        else if (a == "--quiet") opt.quiet = true;
        else if (a == "--animate" && i + 1 < argc) { opt.delayMs = std::atoi(argv[++i]); opt.clearScreen = true; }
        else if (a.rfind("--", 0) == 0) { std::cerr << "Opsi tidak dikenal: " << a << "\n"; printUsage(argv[0]); return 2; }
        else configPath = a;
    }

    try {
        SimulationConfig cfg = configPath.empty() ? SimulationConfig{} : ConfigLoader::loadFromFile(configPath);
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
