#include "StackProvider.hpp"

namespace NDummyConcurrency::NFiber {

    LeasedStack IStackProvider::GetStack() {
        DC_PROFILE_SCOPE();
        return LeasedStack(AllocateStack(), this);
    };

}  // namespace NDummyConcurrency::NFiber
