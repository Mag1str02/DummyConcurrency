#include "Utils.hpp"

#include <DummyConcurrency/DummyConcurrency.hpp>

#include <unistd.h>

using namespace DummyConcurrency;

constexpr uint32_t kFibersToCreate = 50'000'000;

StackPool gPool(Fiber::StackSize::Medium);

void Body(RunLoop& loop, uint32_t& counter) {
    Go(loop,
       [&]() {
           if (++counter < kFibersToCreate) {
               Body(loop, counter);
           }
       },
       {.Pool = &gPool});
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
    std::println("Pool size {} ", gPool.Size());
}