#pragma once

namespace DummyConcurrency {

    struct NonCopyable {
        NonCopyable()                                    = default;
        NonCopyable(const NonCopyable& other)            = delete;
        NonCopyable& operator=(const NonCopyable& other) = delete;
    };
    struct NonMovable {
        NonMovable()                              = default;
        NonMovable(NonMovable&& other)            = delete;
        NonMovable& operator=(NonMovable&& other) = delete;
    };

}  // namespace DummyConcurrency