#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Utils/Assert.hpp>

namespace DummyConcurrency {

    template <typename T>
    class StampedPtr {
    public:
        T*       RawPtr = nullptr;
        uint16_t Stamp;

    public:
        StampedPtr() = default;
        StampedPtr(T* p, uint16_t s) : RawPtr(p), Stamp(s) {}

        T* operator->() const { return RawPtr; }
        T& operator*() const { return *RawPtr; }

        explicit operator bool() const { return RawPtr != nullptr; }

        bool operator==(const StampedPtr& other) { return RawPtr == other.RawPtr && Stamp == other.Stamp; }

        StampedPtr IncrementStamp() const {
            DC_ASSERT(Stamp != UINT16_MAX, "Invalid stamp");
            return StampedPtr{RawPtr, Stamp + 1};
        }
        StampedPtr DecrementStamp() const {
            DC_ASSERT(Stamp > 0, "Invalid stamp");
            return StampedPtr{RawPtr, Stamp - 1};
        }

        static constexpr uint64_t kMaxStamp = (1 << (sizeof(Stamp) * 8)) - 1;
    };

    namespace Detail {

        template <typename T>
        class Packer {
        public:
            using PackedStampedPtr = uintptr_t;

            static PackedStampedPtr Pack(StampedPtr<T> stamped_ptr) {
                DC_ASSERT(stamped_ptr.Stamp <= kMaxStamp, "Stamp is larger than maximum possible");
                return ClearFreeBits((PackedStampedPtr)stamped_ptr.RawPtr) | ((PackedStampedPtr)stamped_ptr.Stamp << kFreeBitsShift);
            }
            static StampedPtr<T> Unpack(PackedStampedPtr packed_ptr) { return {GetRawPointer(packed_ptr), GetStamp(packed_ptr)}; }

        private:
            static constexpr size_t           kFreeBits      = 16;
            static constexpr size_t           kFreeBitsShift = 64 - kFreeBits;
            static constexpr PackedStampedPtr kFreeBitsMask  = (PackedStampedPtr)((1 << kFreeBits) - 1) << kFreeBitsShift;
            static constexpr size_t           kMaxStamp      = (1 << kFreeBits) - 1;
            static_assert(kFreeBits == sizeof(StampedPtr<int>::Stamp) * 8);

            static int              GetBit(PackedStampedPtr mask, size_t index) { return (mask >> index) & 1; }
            static PackedStampedPtr SetFreeBits(PackedStampedPtr mask) { return mask | kFreeBitsMask; }
            static PackedStampedPtr ClearFreeBits(PackedStampedPtr mask) { return mask & ~kFreeBitsMask; }

            // https://en.wikipedia.org/wiki/X86-64#Canonical_form_addresses
            static PackedStampedPtr ToCanonicalForm(PackedStampedPtr ptr) {
                if (GetBit(ptr, 47) != 0) {
                    return SetFreeBits(ptr);
                } else {
                    return ClearFreeBits(ptr);
                }
            }

            static uint16_t GetStamp(PackedStampedPtr packed_ptr) { return packed_ptr >> kFreeBitsShift; }
            static T*       GetRawPointer(PackedStampedPtr packed_ptr) { return (T*)ToCanonicalForm(packed_ptr); }
        };

    }  // namespace Detail

    template <typename T> using PackedStampedPtr = typename Detail::Packer<T>::PackedStampedPtr;

    template <typename T>
    PackedStampedPtr<T> Pack(StampedPtr<T> ptr) {
        return Detail::Packer<T>::Pack(ptr);
    }
    template <typename T>
    StampedPtr<T> Unpack(PackedStampedPtr<T> ptr) {
        return Detail::Packer<T>::Unpack(ptr);
    }

    template <typename T>
    class AtomicStampedPtr {
    public:
        AtomicStampedPtr() = default;
        explicit AtomicStampedPtr(StampedPtr<T> ptr) : packed_ptr_(Pack(ptr)) {}

        void Store(StampedPtr<T> ptr) { packed_ptr_.store(Pack(ptr)); }

        StampedPtr<T> Load() const { return Unpack<T>(packed_ptr_.load()); }
        StampedPtr<T> Exchange(StampedPtr<T> target) { return Unpack<T>(packed_ptr_.exchange(Pack(target))); }

        bool CompareExchangeWeak(StampedPtr<T>& expected, StampedPtr<T> desired) {
            PackedStampedPtr<T> expected_packed = Pack(expected);

            bool succeeded = packed_ptr_.compare_exchange_weak(expected_packed, Pack(desired));
            if (!succeeded) {
                expected = Unpack<T>(expected_packed);
            }
            return succeeded;
        }

    private:
        ImplementationLayer::Atomic<PackedStampedPtr<T>> packed_ptr_ = 0;
    };

}  // namespace DummyConcurrency