#pragma once

#include "DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp"

#include <functional>

namespace NDummyConcurrency::NRuntime {

    class IThreadFactory {
    public:
        virtual ~IThreadFactory() = default;

        virtual NImplementationLayer::Thread LaunchThread(std::function<void()> main) = 0;
    };

}  // namespace NDummyConcurrency::NRuntime
