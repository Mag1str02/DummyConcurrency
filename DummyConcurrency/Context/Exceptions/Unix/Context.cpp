#include <DummyConcurrency/Context/Exceptions/Unix/Context.hpp>

#include <stdexcept>
// memcpy
#include <cstring>

namespace __cxxabiv1 {  // NOLINT

    struct __cxa_eh_globals;  // NOLINT

    // NOLINTNEXTLINE
    extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;

}  // namespace __cxxabiv1

namespace NDummyConcurrency::NContext {

    void ExceptionsContext::SwitchTo(ExceptionsContext& target) {
        static constexpr size_t kSize = sizeof(ExceptionsContext);

        auto* eh = __cxxabiv1::__cxa_get_globals();
        memcpy(buffer_, eh, kSize);
        memcpy(eh, target.buffer_, kSize);
    }

}  // namespace NDummyConcurrency::NContext
