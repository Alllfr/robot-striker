#include <iostream>

#include "test_framework.hpp"

int main() {
    size_t failed = 0;
    for (const auto& t : testRegistry()) {
        try {
            t.fn();
            std::cout << "[ PASS ] " << t.name << "\n";
        } catch (const TestFailure& f) {
            ++failed;
            std::cout << "[ FAIL ] " << t.name << "\n         " << f.message << "\n";
        } catch (const std::exception& e) {
            ++failed;
            std::cout << "[ FAIL ] " << t.name << "\n         exception tak terduga: " << e.what() << "\n";
        }
    }
    std::cout << "\n" << (testRegistry().size() - failed) << "/" << testRegistry().size() << " test lulus\n";
    return failed == 0 ? 0 : 1;
}
