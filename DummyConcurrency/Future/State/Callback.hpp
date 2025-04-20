#pragma once

#include <functional>

namespace DummyConcurrency::Future {

    template <typename T>
    using Callback = std::function<void(T)>;

}  // namespace DummyConcurrency::Future
