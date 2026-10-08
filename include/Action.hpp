#pragma once
#include <string>

enum class ActionType { WAIT, MOVE_FORWARD, TURN, KICK };

// Tiga petak di depan robot -> tiga arah tendangan.
enum class KickDirection { LEFT_DIAGONAL, STRAIGHT, RIGHT_DIAGONAL };

inline double kickAngleOffset(KickDirection k) {
    switch (k) {
        case KickDirection::LEFT_DIAGONAL: return 45.0;
        case KickDirection::RIGHT_DIAGONAL: return -45.0;
        default: return 0.0;
    }
}

// Perintah yang dikirim robot ke Simulator pada tahap ACT.
struct Action {
    ActionType type = ActionType::WAIT;
    double amount = 0.0;  // MOVE_FORWARD: meter, TURN: derajat (+ = kiri)
    KickDirection kickDirection = KickDirection::STRAIGHT;

    static Action wait();
    static Action moveForward(double meters = 0.5);
    static Action turn(double degrees);
    static Action kick(KickDirection direction);

    std::string toString() const;
};
