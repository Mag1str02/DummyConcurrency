#include <DummyConcurrency/DummyConcurrency.hpp>
#include <Testing/BenchTimeBudget.hpp>

#include <thread>

#include <unistd.h>

using namespace DummyConcurrency;

int main() {
    std::cerr << getpid() << std::endl;
    std::getchar();

    uint32_t                 thread_count = std::thread::hardware_concurrency();
    ThreadPool               tp(2);
    FMutex                   mutex;
    TWaitGroup               wg;
    uint32_t                 counter = 0;

    wg.Add(thread_count);
    for (uint32_t i = 0; i < thread_count; ++i) {
        Go(tp, [&]() {
            while (budget) {
                mutex.Lock();
                ++counter;
                mutex.Unlock();
            }
            wg.Done();
        });
    }
    std::cout << "Starting benchmark..." << std::endl;
    tp.Start();
    wg.Wait();
    tp.Stop();
    std::cout << "Critical section amount: " << counter << std::endl;
}