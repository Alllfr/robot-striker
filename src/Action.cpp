#include "Action.hpp"

#include <sstream>

Action Action::wait() { return {}; }
Action Action::moveForward(double meters) { return {ActionType::MOVE_FORWARD, meters, KickDirection::STRAIGHT}; }
Action Action::turn(double degrees) { return {ActionType::TURN, degrees, KickDirection::STRAIGHT}; }
Action Action::kick(KickDirection direction) { return {ActionType::KICK, 0.0, direction}; }

std::string Action::toString() const {
    std::ostringstream os;
    switch (type) {
        case ActionType::WAIT: os << "WAIT"; break;
        case ActionType::MOVE_FORWARD: os << "MOVE_FORWARD(" << amount << " m)"; break;
        case ActionType::TURN: os << "TURN(" << amount << " deg)"; break;
        case ActionType::KICK:
            os << "KICK(";
            switch (kickDirection) {
                case KickDirection::LEFT_DIAGONAL: os << "kiri-diagonal"; break;
                case KickDirection::STRAIGHT: os << "lurus"; break;
                case KickDirection::RIGHT_DIAGONAL: os << "kanan-diagonal"; break;
            }
            os << ")";
            break;
    }
    return os.str();
}
