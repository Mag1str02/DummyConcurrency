#include "StackProvider.hpp"

namespace NDummyConcurrency::NFiber {

    LeasedStack IStackProvider::GetStack() {
        return LeasedStack(AllocateStack(), this);
    };

}  // namespace NDummyConcurrency::NFiber
