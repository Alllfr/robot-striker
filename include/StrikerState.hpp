#pragma once
#include <optional>

#include "Action.hpp"

class Striker;

// Interface State Pattern. Tiap state memutuskan aksi untuk tick ini.
// Mengembalikan nullopt = "saya baru pindah state / belum ada aksi, jalankan state berikutnya".
class StrikerState {
public:
    virtual ~StrikerState() = default;
    virtual std::optional<Action> update(Striker& striker) = 0;
    virtual const char* name() const = 0;
};
