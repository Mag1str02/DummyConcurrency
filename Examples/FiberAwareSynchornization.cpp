
#include "DummyConcurrency/DummyConcurrency.hpp"

using namespace std::chrono_literals;
using namespace DummyConcurrency;

int main() {
    FEvent     event;
    ThreadPool pool(2);

    Fiber::Go(pool, [&]() {
        std::println("Hello from fiber");
        event.Wait();
    });
    Scheduler::Submit(pool, [&]() {
        std::println("Hello from thread {}", std::this_thread::get_id());
        event.Wait();
    });
    pool.Start();

    std::this_thread::sleep_for(10s);

    event.Fire();
    std::cout << "Event fired" << std::endl;

    pool.Stop();
    std::cout << "Pool stoped" << std::endl;

    return 0;
}