#include "Utils.hpp"

#include <DummyConcurrency/DummyConcurrency.hpp>

#include <unistd.h>
#include <thread>

using namespace DummyConcurrency;
using namespace std::chrono_literals;

int main() {
    std::cerr << getpid() << std::endl;
    std::getchar();

    uint32_t   thread_count = std::thread::hardware_concurrency();
    ThreadPool tp(8);
    FMutex mutex;
    TWaitGroup wg;
    uint32_t   counter = 0;

    constexpr uint64_t kSectionsPerThread = 1'000'000;

    wg.Add(thread_count);
    for (uint32_t i = 0; i < thread_count; ++i) {
        Go(tp, [&]() {
            for (uint64_t i = 0; i < kSectionsPerThread; ++i) {
                mutex.lock();
                ++counter;
                mutex.unlock();
            }
            wg.Done();
        });
    }
    std::println("Starting benchmark...");
    Timer t;
    tp.Start();
    wg.Wait();
    tp.Stop();
    std::println("Critical section amount {}", thread_count * kSectionsPerThread);
    std::println("Avg time per section {} ns", t.GetDuration() / (thread_count * kSectionsPerThread) * 1'000'000'000);
}