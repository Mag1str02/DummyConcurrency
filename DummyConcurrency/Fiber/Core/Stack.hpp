#pragma once

// #if defined(__TWIST_SIM_ISOLATION__) || __has_feature(address_sanitizer) || !UNIX

#include <DummyConcurrency/Context/Stack/New.hpp>

namespace DummyConcurrency::Fiber {

    // Adapt to deterministic simulation
    using Stack = Context::NewStack;

}  // namespace DummyConcurrency::Fiber

// #else

// #include <DummyConcurrency/Context/Stack/MMap.hpp>
// #include <DummyConcurrency/Context/Stack/New.hpp>

// namespace DummyConcurrency::Fiber {

//     using Stack = Context::NewStack;

// }  // namespace DummyConcurrency::Fiber

// #endif
