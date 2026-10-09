#pragma once
#include <string>

#include "Ball.hpp"
#include "Field.hpp"
#include "Robot.hpp"

// Menggambar grid ASCII: R robot, O bola, @ area pandang, . kosong, # gawang.
// Setiap elemen dipisahkan spasi. Baris teratas = y terbesar (utara).
class Renderer {
public:
    static std::string render(const Field& field, const Robot& robot, const Ball& ball);
};
