#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Utils/Assert.hpp>
#include <DummyConcurrency/Utils/StampedPtr.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <cstdlib>  // std::abort
#include <print>

//////////////////////////////////////////////////////////////////////

namespace NDummyConcurrency::NDataStructures {

    namespace Detail {

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
            NImplementationLayer::Atomic<uint64_t> state_ = 0;
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
        AtomicSharedPtr() : ptr_(StampedPtr<Container>(nullptr, 0)) {}

        ~AtomicSharedPtr() {
            StampedPtr<Container> ptr = ptr_.Load();
            if (ptr.RawPtr != nullptr) {
                ptr.RawPtr->ModifyState(-1, ptr.Stamp);
            }
        }

        SharedPtr<T> Load() {
            StampedPtr<Container> current = ptr_.Load();
            StampedPtr<Container> new_ptr = current;
            do {
                if (current.RawPtr == nullptr) {
                    break;
                }
                new_ptr.RawPtr = current.RawPtr;
                new_ptr.Stamp  = current.Stamp + 1;
            } while (!ptr_.CompareExchangeWeak(current, new_ptr));
            SharedPtr<T> res;
            res.ptr_ = current.RawPtr;
            if (current.RawPtr != nullptr && new_ptr.Stamp > StampedPtr<Container>::kMaxStamp / 2) {
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
            StampedPtr<Container> new_ptr(target.ptr_, 0);
            StampedPtr<Container> old = ptr_.Exchange(new_ptr);
            if (old.RawPtr != nullptr) {
                old.RawPtr->ModifyState(-1, old.Stamp);
            }
        }

        explicit operator SharedPtr<T>() { return Load(); }

        bool CompareExchangeWeak(SharedPtr<T>& expected, SharedPtr<T> desired) {
            StampedPtr<Container> current = ptr_.Load();
            if (desired.ptr_ != nullptr) {
                desired.ptr_->ModifyState(1, 0);
            }
            StampedPtr<Container> new_ptr;
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
        AtomicStampedPtr<Container> ptr_;
    };

}  // namespace NDummyConcurrency::NDataStructures
