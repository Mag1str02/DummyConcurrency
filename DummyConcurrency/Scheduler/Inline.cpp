#include "Inline.hpp"

namespace DummyConcurrency::Scheduler {

    class Inline : public IScheduler {
    public:
        virtual void Submit(ITask* task) override { task->Run(); }
    };

    IScheduler& Inline() {
        static class Inline sInstance;
        return sInstance;
    }

}  // namespace DummyConcurrency::Scheduler
