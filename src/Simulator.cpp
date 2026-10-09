#include "Simulator.hpp"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <thread>

#include "Exceptions.hpp"
#include "Renderer.hpp"
#include "Striker.hpp"

namespace {
math::Vec2 validatedCenter(const Field& f, const math::Vec2& p, const char* what) {
    if (!f.contains(p)) throw ConfigException(std::string("Posisi ") + what + " di luar lapangan");
    return f.cellCenter(f.cellOf(p));
}
}  // namespace

Simulator::Simulator(const SimulationConfig& config)
    : config_(config), ball_(validatedCenter(field_, config.ballPosition, "bola")) {
    math::Vec2 start = validatedCenter(field_, config.strikerPosition, "Striker");
    if (field_.cellOf(start) == field_.cellOf(ball_.getPosition()))
        throw ConfigException("Striker dan bola tidak boleh di petak yang sama");
    robot_ = std::make_unique<Striker>(field_, start, config.strikerHeading);
}

void Simulator::executeAction(const Action& a) {
    Robot& r = *robot_;
    switch (a.type) {
        case ActionType::WAIT:
            r.setSpeed(0.0);
            break;

        case ActionType::MOVE_FORWARD: {
            if (a.amount > Robot::kMaxSpeed + 1e-9)
                throw SpeedLimitException("Maju " + std::to_string(a.amount) + " m melebihi batas 0.5 m/tick");
            if (std::fabs(a.amount - Field::kCellSize) > 1e-9)
                throw InvalidActionException("Langkah harus tepat 1 petak (0.5 m)");
            Cell next = r.frontCell();
            if (!field_.containsCell(next)) throw OutOfBoundsException("Langkah akan keluar lapangan");
            if (next == field_.cellOf(ball_.getPosition())) throw BlockedException("Petak depan ditempati bola");
            r.setPosition(field_.cellCenter(next));
            r.setSpeed(a.amount);
            break;
        }

        case ActionType::TURN:
            if (std::fabs(a.amount) > Robot::kMaxTurn + 1e-9)
                throw TurnLimitException("Putaran melebihi 90 derajat/tick");
            r.setHeading(r.getHeading() + a.amount);   // setter menolak jika bukan kelipatan 90
            r.setSpeed(0.0);
            break;

        case ActionType::KICK: {
            if (field_.cellOf(ball_.getPosition()) != r.frontCell())
                throw KickNotPossibleException("Bola tidak berada tepat di petak depan robot");
            ball_.kick(math::fromAngle(r.getHeading() + kickAngleOffset(a.kickDirection)));
            r.setSpeed(0.0);
            break;
        }
    }
}

void Simulator::step() {
    if (outcome_ != Outcome::RUNNING) return;
    ++tick_;
    lastError_.clear();

    robot_->sense(ball_);                 // SENSE
    lastAction_ = robot_->think();        // THINK
    try {                                 // ACT (divalidasi simulator)
        executeAction(lastAction_);
    } catch (const InvalidActionException& e) {
        lastError_ = e.what();
        robot_->setSpeed(0.0);
        robot_->onActionRejected(e);
    }

    ball_.update(field_);
    if (ball_.isInGoal()) outcome_ = Outcome::GOAL;
    else if (robot_->hasGivenUp()) outcome_ = Outcome::GAVE_UP;
    else if (tick_ >= config_.maxTicks) outcome_ = Outcome::TIMEOUT;
}

std::string Simulator::frame() const {
    std::ostringstream os;
    os << std::fixed << std::setprecision(2);
    os << "=== Tick " << tick_ << " | " << robot_->stateName() << " | " << lastAction_.toString()
       << " | Robot (" << robot_->getPosition().x << ", " << robot_->getPosition().y << ") hdg "
       << static_cast<int>(robot_->getHeading()) << " | Bola (" << ball_.getPosition().x << ", "
       << ball_.getPosition().y << ") ===\n";
    if (!lastError_.empty()) os << "!! Aksi ditolak: " << lastError_ << "\n";
    os << Renderer::render(field_, *robot_, ball_);
    return os.str();
}

Outcome Simulator::run(std::ostream& out, const RunOptions& opt) {
    auto show = [&](bool force) {
        if (opt.quiet && !force) return;
        if (opt.clearScreen) out << "\033[H\033[J";
        out << frame() << std::flush;
        if (opt.delayMs > 0 && !force) std::this_thread::sleep_for(std::chrono::milliseconds(opt.delayMs));
    };
    show(false);
    while (outcome_ == Outcome::RUNNING) {
        step();
        show(false);
    }
    if (opt.quiet) show(true);
    switch (outcome_) {
        case Outcome::GOAL: out << "GOL! Bola masuk gawang pada tick " << tick_ << ".\n"; break;
        case Outcome::TIMEOUT: out << "Waktu habis (" << tick_ << " tick) tanpa gol.\n"; break;
        case Outcome::GAVE_UP: out << "Striker menyerah: bola terjebak (tidak ada tendangan yang berguna).\n"; break;
        default: break;
    }
    return outcome_;
}
