#include "Utils.hpp"

#include <DummyConcurrency/DummyConcurrency.hpp>
#include <Testing/BenchTimeBudget.hpp>

#include <thread>

#include <unistd.h>

using namespace DummyConcurrency;

int main() {
    std::cerr << getpid() << std::endl;
    std::getchar();

    RunLoop loop;
    Go(loop, [&]() {
        for (uint32_t i = 0; i < 100'000'000; ++i) {
            Yield();
        }
    });

    std::cout << "Starting benchmark..." << std::endl;
    Timer timer;
    loop.Run();
    std::cout << "Time passed: " << timer.GetDuration() << std::endl;
}