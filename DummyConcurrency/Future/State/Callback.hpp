#pragma once

#include <functional>

namespace NDummyConcurrency::NFuture {

    template <typename T>
    using Callback = std::function<void(T&&)>;

}  // namespace NDummyConcurrency::NFuture
