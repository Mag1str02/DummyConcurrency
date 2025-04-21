
#include "DummyConcurrency/DummyConcurrency.hpp"

using namespace std::chrono_literals;
using namespace DummyConcurrency;

int main() {
    ThreadPool pool(16);
    TEvent     event;

    auto func = [&]() {
        std::cout << "Hi from thread: " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(20s);
        event.Fire();
        std::cout << "Bye from thread: " << std::this_thread::get_id() << std::endl;
    };
    Submit(pool, func);

    pool.Start();
    event.Wait();
    std::cerr << "Event waited" << std::endl;
    pool.Stop();
    return 0;
}