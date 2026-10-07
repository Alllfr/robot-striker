#include <stdexcept>

#include "Field.hpp"
#include "test_framework.hpp"

TEST(field_grid_dimensions) {
    CHECK(Field::kCols == 18);
    CHECK(Field::kRows == 12);
}

TEST(field_cell_mapping) {
    Field f;
    Cell a = f.cellOf({-4.5, -3.0});
    CHECK(a.col == 0 && a.row == 0);
    Cell b = f.cellOf({4.5, 3.0});  // tepat di batas -> di-clamp ke petak terakhir
    CHECK(b.col == 17 && b.row == 11);
    Cell c = f.cellOf({0.1, 0.1});
    CHECK(c.col == 9 && c.row == 6);
}

TEST(field_cell_center_roundtrip) {
    Field f;
    math::Vec2 c = f.cellCenter({0, 0});
    CHECK_NEAR(c.x, -4.25, 1e-9);
    CHECK_NEAR(c.y, -2.75, 1e-9);
    for (int col = 0; col < Field::kCols; ++col)
        for (int row = 0; row < Field::kRows; ++row) {
            Cell cell{col, row};
            CHECK(f.cellOf(f.cellCenter(cell)) == cell);
        }
    CHECK_THROWS(f.cellCenter({18, 0}), std::out_of_range);
}

TEST(field_contains) {
    Field f;
    CHECK(f.contains({0, 0}));
    CHECK(f.contains({4.5, 3.0}));
    CHECK(!f.contains({4.6, 0}));
    CHECK(!f.contains({0, -3.1}));
}

TEST(field_goal_is_three_meters_wide) {
    Field f;
    int goalRows = 0;
    for (int r = 0; r < Field::kRows; ++r)
        if (f.isGoalRow(r)) ++goalRows;
    CHECK(goalRows == 6);  // 6 petak * 0.5 m = 3 m
    CHECK_NEAR(f.goalLineX(), 4.5, 1e-9);
}
