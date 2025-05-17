#include "Inline.hpp"

namespace NDummyConcurrency::NRuntime {

    class Inline : public IScheduler {
    public:
        virtual void Submit(ITask* task) override {
            DC_PROFILE_SCOPE();
            task->Run();
        }
    };

    IScheduler& InlineScheduler() {
        static class Inline sInstance;
        return sInstance;
    }

}  // namespace NDummyConcurrency::NRuntime
