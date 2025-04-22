
#include "DummyConcurrency/DummyConcurrency.hpp"

#include <format>

using namespace std::chrono_literals;
using namespace NDummyConcurrency;

int main() {
    NRuntime::RunLoop loop;
    NFiber::Go(loop, []() {
        std::cout << "Hello from fiber 1: " << std::this_thread::get_id() << std::endl;
        NFiber::Yield();
        std::cout << "Hello from fiber 3: " << std::this_thread::get_id() << std::endl;
        NFiber::Yield();
        std::cout << "Hello from fiber 5: " << std::this_thread::get_id() << std::endl;
    });

    std::cout << "Begging in thread: " << std::this_thread::get_id() << std::endl;
    loop.RunNext();
    std::cout << "Hello from fiber 2: " << std::this_thread::get_id() << std::endl;
    loop.RunNext();
    std::cout << "Hello from fiber 4: " << std::this_thread::get_id() << std::endl;
    loop.RunNext();
    std::cout << "Hello from fiber 6: " << std::this_thread::get_id() << std::endl;
    loop.RunNext();

    return 0;
}