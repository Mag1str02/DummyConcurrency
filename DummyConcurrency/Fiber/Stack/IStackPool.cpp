#include "IStackPool.hpp"

namespace NDummyConcurrency::NFiber {

    LeasedStack IStackPool::GetStack() {
        return LeasedStack(AllocateStack(), this);
    };

}  // namespace NDummyConcurrency::NFiber
