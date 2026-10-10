#pragma once
#include "Field.hpp"
#include "MathUtils.hpp"

// Hasil prediksi satu tendangan (dipakai planner Striker dari model fisika yang sama dengan simulator).
struct KickPrediction {
    math::Vec2 finalPosition;
    bool goal = false;
    bool outOfBounds = false;   // bola keluar lapangan -> respawn di tengah
    int ticks = 0;
};

// Bola: ditendang dengan kecepatan awal 3 m/tick, melambat 1 m/tick tiap tick
// (jarak per tick: 3, 2, 1 -> total 6 m).
// Bola yang keluar lapangan (bukan lewat gawang) langsung respawn di petak tengah lapangan.
class Ball {
public:
    static constexpr double kKickSpeed = 3.0;
    static constexpr double kDeceleration = 1.0;

    explicit Ball(const math::Vec2& position = {});

    const math::Vec2& getPosition() const { return position_; }
    void setPosition(const math::Vec2& p) { position_ = p; }
    double getSpeed() const { return speed_; }
    bool isMoving() const { return speed_ > 0.0; }
    bool isInGoal() const { return inGoal_; }
    // true sekali setelah bola keluar lapangan & di-respawn (lalu otomatis direset).
    bool consumeOutOfBounds() { bool v = outOfBounds_; outOfBounds_ = false; return v; }

    void kick(const math::Vec2& direction);  // throw std::invalid_argument jika arah nol
    void update(const Field& field);         // maju satu tick

    // Simulasikan tendangan dari `start` tanpa mengubah bola asli.
    static KickPrediction predictKick(const Field& field, const math::Vec2& start,
                                      const math::Vec2& direction);

private:
    struct Step {
        math::Vec2 position;
        bool hitWall = false;
        bool goal = false;
    };
    static Step advance(const Field& field, const math::Vec2& from, const math::Vec2& dir, double dist);

    math::Vec2 position_;
    math::Vec2 direction_{1.0, 0.0};
    double speed_ = 0.0;
    bool inGoal_ = false;
    bool outOfBounds_ = false;
};