
#include "DummyConcurrency/DummyConcurrency.hpp"

using namespace std::chrono_literals;
using namespace DummyConcurrency;

int main() {
    Scheduler::RunLoop loop;
    Fiber::Go(loop, []() {
        std::println("Hello from fiber 1: {}", std::this_thread::get_id());
        Fiber::Yield();
        std::println("Hello from fiber 3: {}", std::this_thread::get_id());
        Fiber::Yield();
        std::println("Hello from fiber 5: {}", std::this_thread::get_id());
    });

    std::println("Begining: {}", std::this_thread::get_id());
    loop.RunNext();
    std::println("Hello from main 2: {}", std::this_thread::get_id());
    loop.RunNext();
    std::println("Hello from main 4: {}", std::this_thread::get_id());
    loop.RunNext();
    std::println("Hello from main 6: {}", std::this_thread::get_id());
    loop.RunNext();

    return 0;
}