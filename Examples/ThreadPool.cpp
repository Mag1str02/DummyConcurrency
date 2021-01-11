
#include "DummyConcurrency/DummyConcurrency.hpp"

using namespace std::chrono_literals;
using namespace DummyConcurrency;

int main() {
    ThreadPool pool(2);

    auto func = []() {
        std::println("Hi from thread: {}", std::this_thread::get_id());
        std::this_thread::sleep_for(1s);
        std::println("Buy from thread: {}", std::this_thread::get_id());
    };
    Submit(pool, func);

    pool.Start();
    pool.Stop();
    return 0;
}