#include "Renderer.hpp"

#include <sstream>
#include <vector>

std::string Renderer::render(const Field& field, const Robot& robot, const Ball& ball) {
    std::vector<std::vector<char>> grid(Field::kRows, std::vector<char>(Field::kCols, '.'));

    for (const Cell& c : robot.getSensor().visibleCells(field, robot.getPose())) grid[c.row][c.col] = '@';
    Cell b = field.cellOf(ball.getPosition());
    grid[b.row][b.col] = 'O';
    Cell r = robot.getCell();
    grid[r.row][r.col] = 'R';

    std::ostringstream os;
    for (int row = Field::kRows - 1; row >= 0; --row) {
        for (int col = 0; col < Field::kCols; ++col) {
            if (col) os << ' ';
            os << grid[row][col];
        }
        if (field.isGoalRow(row)) os << " #";   // gawang di garis x = 4.5
        os << '\n';
    }
    return os.str();
}
