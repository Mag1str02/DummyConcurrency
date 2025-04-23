#pragma once

namespace NDummyConcurrency::NContext {

    // https://refspecs.linuxfoundation.org/abi-eh-1.22.html

    class ExceptionsContext {
    public:
        void SwitchTo(ExceptionsContext& target);

    private:
        void* buffer_[2] = {0, 0};
    };

}  // namespace NDummyConcurrency::NContext
