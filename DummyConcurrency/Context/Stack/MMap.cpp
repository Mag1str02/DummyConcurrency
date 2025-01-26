#include <DummyConcurrency/Context/Stack/MMap.hpp>

#include <sys/mman.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <utility>

namespace DummyConcurrency::Context {

    namespace {
        class PageSizeDetector {
        public:
            PageSizeDetector() { page_size_ = DetectPageSize(); }

            size_t GetPageSize() const { return page_size_; }

        private:
            size_t DetectPageSize() { return sysconf(_SC_PAGESIZE); }

        private:
            size_t page_size_;
        };

        size_t GetCachedPageSize() {
            static PageSizeDetector sPageSizeDetector;

            return sPageSizeDetector.GetPageSize();
        }

    }  // namespace

    MmapStack::MmapStack(MmapAllocation allocation) : allocation_(std::move(allocation)) {}

    MmapStack MmapStack::AllocatePages(size_t count) {
        auto allocation = MmapAllocation::AllocatePages(count + 1);
        // Guard page
        allocation.ProtectPages(/*offset=*/0, /*count=*/1);
        return MmapStack{std::move(allocation)};
    }

    MmapStack MmapStack::AllocateBytes(size_t at_least) {
        const size_t page_size = MmapAllocation::PageSize();

        size_t pages = at_least / page_size;
        if (at_least % page_size != 0) {
            ++pages;
        }

        return MmapStack::AllocatePages(/*count=*/pages);
    }

    void* MmapStack::LowerBound() const {
        return allocation_.Start() + MmapAllocation::PageSize();
    }

    static StackView ToSpan(MutableMemView mem_view) {
        return {(std::byte*)mem_view.Data(), mem_view.Size()};
    }

    static MutableMemView ToMemView(StackView stack) {
        return {(char*)stack.data(), stack.size()};
    }

    StackView MmapStack::MutView() {
        return ToSpan(allocation_.MutView());
    }

    MmapStack MmapStack::Acquire(StackView view) {
        return MmapStack(MmapAllocation::Acquire(ToMemView(view)));
    }

    StackView MmapStack::Release() {
        return ToSpan(allocation_.Release());
    }

#define CHECK_RESULT(ret, error) DC_ASSERT(ret != -1, error + " (errno = " + std::to_string(errno) + ", " + strerror(errno) + ")")

    //////////////////////////////////////////////////////////////////////

    static size_t PagesToBytes(size_t count) {
        return count * MmapAllocation::PageSize();
    }

    size_t MmapAllocation::PageSize() {
        return GetCachedPageSize();
    }

    MmapAllocation MmapAllocation::AllocatePages(size_t count, void* hint) {
        size_t size = PagesToBytes(count);

        void* start = mmap(/*addr=*/hint,
                           /*length=*/size,
                           /*prot=*/PROT_READ | PROT_WRITE,
                           /*flags=*/MAP_PRIVATE | MAP_ANONYMOUS,
                           /*fd=*/-1,
                           /*offset=*/0);

        DC_ASSERT(start != MAP_FAILED, std::string("Cannot allocate ") + std::to_string(count) + " pages");

        return MmapAllocation{(char*)start, size};
    }

    MmapAllocation MmapAllocation::Acquire(MutableMemView view) {
        // TODO: check size and alignment
        return MmapAllocation{view.Data(), view.Size()};
    }

    void MmapAllocation::ProtectPages(size_t start_index, size_t count) {
        int ret = mprotect(/*addr=*/(void*)(start_ + PagesToBytes(start_index)),
                           /*len=*/PagesToBytes(count),
                           /*prot=*/PROT_NONE);
        CHECK_RESULT(ret, std::string("Cannot protect pages [") + std::to_string(start_index) + ", " + std::to_string(start_index + count) + ")");
    }

    MmapAllocation::MmapAllocation(MmapAllocation&& that) {
        start_ = that.start_;
        size_  = that.size_;
        that.Reset();
    }

    MmapAllocation& MmapAllocation::operator=(MmapAllocation&& that) {
        Deallocate();
        start_ = that.start_;
        size_  = that.size_;
        that.Reset();
        return *this;
    }

    void MmapAllocation::Deallocate() {
        if (start_ == nullptr) {
            return;
        }

        int ret = munmap((void*)start_, size_);
        CHECK_RESULT(ret, std::string("Cannot unmap allocated pages"));
    }

    MutableMemView MmapAllocation::Release() {
        auto view = MutView();
        Reset();
        return view;
    }

    void MmapAllocation::Reset() {
        start_ = nullptr;
        size_  = 0;
    }

}  // namespace DummyConcurrency::Context
