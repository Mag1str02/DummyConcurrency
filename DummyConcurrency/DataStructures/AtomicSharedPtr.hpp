#pragma once

#include <DummyConcurrency/Common.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <twist/ed/std/atomic.hpp>

#include <twist/assist/memory.hpp>
#include <twist/trace/scope.hpp>

#include <cassert>
#include <cstdlib>  // std::abort
#include <print>

//////////////////////////////////////////////////////////////////////

namespace DummyConcurrency::DataStructures {

    namespace Detail {

        template <typename T>
        class StampedPtr {
        public:
            StampedPtr() = default;
            StampedPtr(T* p, uint64_t s) : RawPtr(p), Stamp(s) {}

            T* operator->() const { return RawPtr; }
            T& operator*() const { return *RawPtr; }

            explicit operator bool() const { return RawPtr != nullptr; }

            bool operator==(const StampedPtr& other) { return RawPtr == other.RawPtr && Stamp == other.Stamp; }

            StampedPtr IncrementStamp() const { return StampedPtr{RawPtr, Stamp + 1}; }
            StampedPtr DecrementStamp() const {
                DC_ASSERT(Stamp > 0, "Invalid stamp");
                return StampedPtr{RawPtr, Stamp - 1};
            }

        public:
            T*       RawPtr;
            uint64_t Stamp;
        };

        template <typename T>
        class Packer {
        public:
            using PackedStampedPtr = uintptr_t;

            static constexpr size_t    kFreeBits      = 16;
            static constexpr size_t    kFreeBitsShift = 64 - kFreeBits;
            static constexpr uintptr_t kFreeBitsMask  = (uintptr_t)((1 << kFreeBits) - 1) << kFreeBitsShift;
            static constexpr size_t    kMaxStamp      = (1 << kFreeBits) - 1;

            static PackedStampedPtr Pack(StampedPtr<T> stamped_ptr) {
                DC_ASSERT(stamped_ptr.Stamp <= kMaxStamp, "Stamp is larger than maximum possible");
                return ClearFreeBits((uintptr_t)stamped_ptr.RawPtr) | (stamped_ptr.Stamp << kFreeBitsShift);
            }
            static StampedPtr<T> Unpack(PackedStampedPtr packed_ptr) { return {GetRawPointer(packed_ptr), GetStamp(packed_ptr)}; }

        private:
            static int       GetBit(uintptr_t mask, size_t index) { return (mask >> index) & 1; }
            static uintptr_t SetFreeBits(uintptr_t mask) { return mask | kFreeBitsMask; }
            static uintptr_t ClearFreeBits(uintptr_t mask) { return mask & ~kFreeBitsMask; }

            // https://en.wikipedia.org/wiki/X86-64#Canonical_form_addresses
            static uintptr_t ToCanonicalForm(uintptr_t ptr) {
                if (GetBit(ptr, 47) != 0) {
                    return SetFreeBits(ptr);
                } else {
                    return ClearFreeBits(ptr);
                }
            }

            static uint64_t GetStamp(PackedStampedPtr packed_ptr) { return packed_ptr >> kFreeBitsShift; }
            static T*       GetRawPointer(PackedStampedPtr packed_ptr) { return (T*)ToCanonicalForm(packed_ptr); }
        };

        template <typename T> using PackedStampedPtr = typename Detail::Packer<T>::PackedStampedPtr;

        template <typename T>
        PackedStampedPtr<T> Pack(StampedPtr<T> ptr) {
            return Detail::Packer<T>::Pack(ptr);
        }
        template <typename T>
        StampedPtr<T> Unpack(PackedStampedPtr<T> ptr) {
            return Detail::Packer<T>::Unpack(ptr);
        }

        //////////////////////////////////////////////////////////////////////

        // 48-bit pointer + 16-bit stamp packed into single 64-bit word

        // Usage:

        // asp.Store({raw_ptr, 42});
        // auto ptr = asp.Load();
        // if (asp) { ... }
        // asp->Foo();
        // asp.CompareExchangeWeak(expected_stamped_ptr, {raw_ptr, 42});

        // auto e =
        // top_.compare_exchange(target.Pack(),

        template <typename T>
        class AtomicStampedPtr {
        public:
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
            ImplementationLayer::Atomic<PackedStampedPtr<T>> packed_ptr_;
        };

        template <typename T>
        class DiffCounted : NonMovable, NonCopyable {
        public:
            template <typename... Args>
            explicit DiffCounted(Args&&... args) : value_(std::forward<Args>(args)...) {
                State state;
                state.Counters.Basic  = 1;
                state.Counters.Strong = 0;

                state_ = state.UI64;
            }
            T* Get() { return &value_; }

            void ModifyState(int32_t strong, int32_t basic) {
                State latest, new_state;
                latest.UI64 = state_.load();
                do {
                    // TODO: Overflow checks
                    new_state = latest;
                    new_state.Counters.Strong += strong;
                    new_state.Counters.Basic += basic;
                } while (!state_.compare_exchange_weak(latest.UI64, new_state.UI64));
                if (new_state.UI64 == 0) {
                    delete this;
                }
            }

        private:
            struct Counters {
                uint32_t Strong;
                int32_t  Basic;
            };
            union State {
                uint64_t UI64;
                Counters Counters;
            };
            static_assert(sizeof(State) == sizeof(uint64_t));

        private:
            ImplementationLayer::Atomic<uint64_t> state_ = 0;
            T                                     value_;
        };
    }  // namespace Detail

    //////////////////////////////////////////////////////////////////////

    template <typename T>
    class SharedPtr {
    public:
        SharedPtr() = default;

        SharedPtr(SharedPtr&& other) : ptr_(other.ptr_) { other.ptr_ = nullptr; }
        SharedPtr& operator=(SharedPtr&& other) {
            if (&other == this) {
                return *this;
            }

            if (ptr_ != nullptr) {
                ptr_->ModifyState(0, -1);
            }

            ptr_       = other.ptr_;
            other.ptr_ = nullptr;
            return *this;
        }
        SharedPtr(const SharedPtr& other) : ptr_(other.ptr_) {
            if (ptr_) {
                ptr_->ModifyState(0, 1);
            }
        }
        SharedPtr& operator=(const SharedPtr& other) {
            if (&other == this) {
                return *this;
            }

            if (ptr_ != other.ptr_) {
                if (ptr_ != nullptr) {
                    ptr_->ModifyState(0, -1);
                }

                ptr_ = other.ptr_;

                if (ptr_ != nullptr) {
                    ptr_->ModifyState(0, 1);
                }
            }

            return *this;
        }

        T* operator->() const { return ptr_->Get(); }
        T& operator*() const { return *ptr_->Get(); }

        explicit operator bool() const { return ptr_ != nullptr; }

        void Reset() {
            if (ptr_) {
                ptr_->ModifyState(0, -1);
                ptr_ = nullptr;
            }
        }

        ~SharedPtr() { Reset(); }

    private:
        template <typename F, typename... Args>
        friend SharedPtr<F> MakeShared(Args&&...);
        template <typename F>
        friend class AtomicSharedPtr;

        Detail::DiffCounted<T>* ptr_ = nullptr;
    };

    template <typename T, typename... Args>
    SharedPtr<T> MakeShared(Args&&... args) {
        auto*        storage = new Detail::DiffCounted<T>(std::forward<Args>(args)...);
        SharedPtr<T> res;
        res.ptr_ = storage;
        return res;
    }

    //////////////////////////////////////////////////////////////////////

    template <typename T>
    class AtomicSharedPtr : public NonCopyable, public NonMovable {
        using Container = Detail::DiffCounted<T>;

    public:
        // nullptr
        AtomicSharedPtr() : ptr_(Detail::StampedPtr<Container>(nullptr, 0)) {}

        ~AtomicSharedPtr() {
            Detail::StampedPtr<Container> ptr = ptr_.Load();
            if (ptr.RawPtr != nullptr) {
                ptr.RawPtr->ModifyState(-1, ptr.Stamp);
            }
        }

        SharedPtr<T> Load() {
            Detail::StampedPtr<Container> current = ptr_.Load();
            Detail::StampedPtr<Container> new_ptr = current;
            do {
                if (current.RawPtr == nullptr) {
                    break;
                }
                new_ptr.RawPtr = current.RawPtr;
                new_ptr.Stamp  = current.Stamp + 1;
            } while (!ptr_.CompareExchangeWeak(current, new_ptr));
            SharedPtr<T> res;
            res.ptr_ = current.RawPtr;
            if (current.RawPtr != nullptr && new_ptr.Stamp > Detail::Packer<Container>::kMaxStamp / 2) {
                new_ptr.RawPtr->ModifyState(1, 0);
                new_ptr.Stamp = 0;
                current.Stamp = new_ptr.Stamp + 1;
                do {
                    if (new_ptr.RawPtr != current.RawPtr) {
                        break;
                    }
                } while (!ptr_.CompareExchangeWeak(current, new_ptr));
                new_ptr.RawPtr->ModifyState(-1, current.Stamp);
            }

            return res;
        }

        void Store(SharedPtr<T> target) {
            if (target.ptr_ != nullptr) {
                target.ptr_->ModifyState(1, 0);
            }
            Detail::StampedPtr<Container> new_ptr(target.ptr_, 0);
            Detail::StampedPtr<Container> old = ptr_.Exchange(new_ptr);
            if (old.RawPtr != nullptr) {
                old.RawPtr->ModifyState(-1, old.Stamp);
            }
        }

        explicit operator SharedPtr<T>() { return Load(); }

        bool CompareExchangeWeak(SharedPtr<T>& expected, SharedPtr<T> desired) {
            Detail::StampedPtr<Container> current = ptr_.Load();
            if (desired.ptr_ != nullptr) {
                desired.ptr_->ModifyState(1, 0);
            }
            Detail::StampedPtr<Container> new_ptr;
            new_ptr.Stamp  = 0;
            new_ptr.RawPtr = desired.ptr_;
            do {
                if (current.RawPtr != expected.ptr_) {
                    expected = Load();
                    if (desired.ptr_ != nullptr) {
                        desired.ptr_->ModifyState(-1, 0);
                    }
                    return false;
                }
            } while (!ptr_.CompareExchangeWeak(current, new_ptr));
            if (current.RawPtr != nullptr) {
                current.RawPtr->ModifyState(-1, current.Stamp);
            }
            return true;
        }

    private:
        Detail::AtomicStampedPtr<Container> ptr_;
    };

}  // namespace DummyConcurrency::DataStructures
