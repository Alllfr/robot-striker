#include "ConfigLoader.hpp"
#include "Exceptions.hpp"
#include "test_framework.hpp"

TEST(config_parses_valid_text) {
    SimulationConfig c = ConfigLoader::parse("# komentar\nstriker_x = -3.0\nstriker_y=1.0\nstriker_heading = 90\nball_x=0\nball_y=0\nmax_ticks=120\n");
    CHECK_NEAR(c.strikerPosition.x, -3.0, 1e-9);
    CHECK_NEAR(c.strikerHeading, 90.0, 1e-9);
    CHECK(c.maxTicks == 120);
}

TEST(config_rejects_invalid_text) {
    CHECK_THROWS(ConfigLoader::parse("striker_x = abc"), ConfigException);
    CHECK_THROWS(ConfigLoader::parse("unknown_key = 1"), ConfigException);
    CHECK_THROWS(ConfigLoader::parse("ball_x = 9.5"), ConfigException);
    CHECK_THROWS(ConfigLoader::parse("striker_heading = 45"), ConfigException);
    CHECK_THROWS(ConfigLoader::parse("tanpa sama dengan"), ConfigException);
    CHECK_THROWS(ConfigLoader::loadFromFile("/tidak/ada.txt"), ConfigException);
}
