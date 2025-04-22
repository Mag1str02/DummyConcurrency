#include "Utils.hpp"

#include <DummyConcurrency/DummyConcurrency.hpp>

#include <unistd.h>

using namespace DummyConcurrency;

int main() {
    std::cerr << getpid() << std::endl;
    std::getchar();

    constexpr uint64_t kYieldAmount = 100'000'000;

    RunLoop loop;
    Go(loop, [&]() {
        for (uint32_t i = 0; i < kYieldAmount; ++i) {
            Yield();
        }
    });

    std::println("Starting benchmark...");
    Timer timer;
    loop.Run();
    std::println("Performed {} context switches", kYieldAmount * 2);
    std::println("Avg time per switch {} ns", timer.GetDuration() / (kYieldAmount * 2) * 1'000'000'000);
}