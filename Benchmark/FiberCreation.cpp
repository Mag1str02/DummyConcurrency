#include "Utils.hpp"

#include <DummyConcurrency/DummyConcurrency.hpp>

#include <functional>
#include <thread>

#include <unistd.h>

using namespace DummyConcurrency;

constexpr uint32_t kFibersToCreate = 10'000'000;

void Body(RunLoop& loop, uint32_t& counter) {
    Go(loop, [&]() {
        if (++counter < kFibersToCreate) {
            Body(loop, counter);
        }
    });
}

int main() {
    std::cerr << getpid() << std::endl;
    std::getchar();

    RunLoop  loop;
    uint32_t counter = 0;

    Body(loop, counter);

    Timer tm;
    loop.Run();

    std::println("Created {} fibers in {} seconds", kFibersToCreate, tm.GetDuration());
    std::println("Spending {} ns per fibers", tm.GetDuration() / kFibersToCreate * 1'000'000'000);
}