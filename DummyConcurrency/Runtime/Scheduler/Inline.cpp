#include "Inline.hpp"

namespace DummyConcurrency::Runtime {

    class Inline : public IScheduler {
    public:
        virtual void Submit(ITask* task) override { task->Run(); }
    };

    IScheduler& Inline() {
        static class Inline sInstance;
        return sInstance;
    }

}  // namespace DummyConcurrency::Runtime
