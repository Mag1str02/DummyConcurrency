
#include "DummyConcurrency/DummyConcurrency.hpp"

#include <thread>

using namespace std::chrono_literals;
using namespace NDummyConcurrency;

int main() {
    ThreadPool pool(2);

    auto func = []() {
        std::cout << "Hi from thread: {}" << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(1s);
        std::cout << "Bye from thread: {}" << std::this_thread::get_id() << std::endl;
    };
    Submit(pool, func);

    pool.Start();
    pool.Stop();
    return 0;
}