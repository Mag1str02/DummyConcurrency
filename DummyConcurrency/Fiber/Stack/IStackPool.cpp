#include "IStackPool.hpp"

namespace DummyConcurrency::Fiber {

    LeasedStack IStackPool::GetStack() {
        return LeasedStack(AllocateStack(), this);
    };

}  // namespace DummyConcurrency::Fiber
