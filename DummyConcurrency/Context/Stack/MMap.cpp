#include <DummyConcurrency/Context/Stack/MMap.hpp>

#include <utility>

namespace DummyConcurrency::Context {

    MmapStack::MmapStack(tmp_wheels::MmapAllocation allocation) : allocation_(std::move(allocation)) {}

    MmapStack MmapStack::AllocatePages(size_t count) {
        auto allocation = tmp_wheels::MmapAllocation::AllocatePages(count + 1);
        // Guard page
        allocation.ProtectPages(/*offset=*/0, /*count=*/1);
        return MmapStack{std::move(allocation)};
    }

    MmapStack MmapStack::AllocateBytes(size_t at_least) {
        const size_t page_size = tmp_wheels::MmapAllocation::PageSize();

        size_t pages = at_least / page_size;
        if (at_least % page_size != 0) {
            ++pages;
        }

        return MmapStack::AllocatePages(/*count=*/pages);
    }

    void* MmapStack::LowerBound() const {
        return allocation_.Start() + tmp_wheels::MmapAllocation::PageSize();
    }

    static StackView ToSpan(tmp_wheels::MutableMemView mem_view) {
        return {(std::byte*)mem_view.Data(), mem_view.Size()};
    }

    static tmp_wheels::MutableMemView ToMemView(StackView stack) {
        return {(char*)stack.data(), stack.size()};
    }

    StackView MmapStack::MutView() {
        return ToSpan(allocation_.MutView());
    }

    MmapStack MmapStack::Acquire(StackView view) {
        return MmapStack(tmp_wheels::MmapAllocation::Acquire(ToMemView(view)));
    }

    StackView MmapStack::Release() {
        return ToSpan(allocation_.Release());
    }

}  // namespace DummyConcurrency::Context
