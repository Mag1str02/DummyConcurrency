#include "Utils.hpp"

#include <DummyConcurrency/DummyConcurrency.hpp>

#include <unistd.h>

using namespace NDummyConcurrency;

class TestTask : public NRuntime::ITask {
public:
    virtual void Run() noexcept override {
        for (uint32_t i = 0; i < 100; ++i) {}
        delete this;
    };
};

const uint32_t kCount = 100'000'000;

void BenchNormal() {
    TestTask task;
    Timer    t;
    for (uint32_t i = 0; i < kCount; ++i) {
        auto t = new TestTask();
        t->Run();
    }
    std::cout << "For loop count: " << t.GetDuration() << std::endl;
}

void BenchThreadPool() {
    ThreadPool tp(1);
    tp.Start();
    Timer t;
    for (uint32_t i = 0; i < kCount; ++i) {
        tp.Submit(new TestTask());
    }
    tp.Stop();
    std::cout << "Thread pool count: " << t.GetDuration() << std::endl;
}

int main() {
    std::cerr << getpid() << std::endl;
    std::getchar();

    BenchNormal();
    BenchThreadPool();
}