#pragma once
#include <iosfwd>
#include <memory>
#include <string>

#include "Action.hpp"
#include "Ball.hpp"
#include "Field.hpp"
#include "Robot.hpp"
#include "SimulationConfig.hpp"

enum class Outcome { RUNNING, GOAL, TIMEOUT, GAVE_UP };

struct RunOptions {
    int delayMs = 0;          // jeda antar tick (animasi)
    bool clearScreen = false; // bersihkan terminal tiap frame (ANSI)
    bool quiet = false;       // hanya cetak frame akhir
};

// Pengelola game loop berbasis tick (1 tick = 1 detik). Simulator memegang "dunia"
// (Field, Ball) dan memvalidasi setiap aksi robot sebelum menerapkannya.
class Simulator {
public:
    explicit Simulator(const SimulationConfig& config);
    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;

    void step();                                              // 1 tick: Sense -> Think -> Act
    Outcome run(std::ostream& out, const RunOptions& opt = {});

    // Terapkan aksi. Aksi ilegal -> throw InvalidActionException (ditangkap di step()).
    void executeAction(const Action& action);

    int tick() const { return tick_; }
    Outcome outcome() const { return outcome_; }
    const Field& field() const { return field_; }
    const Ball& ball() const { return ball_; }
    const Robot& robot() const { return *robot_; }
    const std::string& lastError() const { return lastError_; }
    std::string frame() const;   // header + grid ASCII tick saat ini

private:
    SimulationConfig config_;
    Field field_;
    Ball ball_;
    std::unique_ptr<Robot> robot_;
    int tick_ = 0;
    Outcome outcome_ = Outcome::RUNNING;
    Action lastAction_;
    std::string lastError_;
};
