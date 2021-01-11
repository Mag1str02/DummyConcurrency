#pragma once

#if defined(__TWIST_SIM_ISOLATION__)

#include <DummyConcurrency/Context/Stack/New.hpp>

namespace DummyConcurrency::Fiber {

    // Adapt to deterministic simulation
    using Stack = Context::NewStack;

}  // namespace DummyConcurrency::Fiber

#elif __has_feature(address_sanitizer)

#include <DummyConcurrency/Context/Stack/New.hpp>

namespace DummyConcurrency::Fiber {

    // Rely on stack-overflow checking in AddressSanitizer
    using Stack = Context::NewStack;

}  // namespace DummyConcurrency::Fiber

#else

#include <DummyConcurrency/Context/Stack/MMap.hpp>

namespace DummyConcurrency::Fiber {

    using Stack = Context::MmapStack;

}  // namespace DummyConcurrency::Fiber

#endif
