#pragma once

#include <DummyConcurrency/Utils/Assert.hpp>

#include <utility>

namespace NDummyConcurrency {

    // Storage for value of type T with manual lifetime management

    template <typename T>
    class ManualLifetime {
    public:
        template <typename... Args>
        explicit ManualLifetime(Args&&... args) {
            Construct(std::forward<Args>(args)...);
        }
        ManualLifetime() = default;
        ~ManualLifetime() {
            if (contructed_) {
                Destruct();
            }
        }

        template <typename... Args>
        void Construct(Args&&... args) {
            contructed_ = true;
            new (&storage_) T(std::forward<Args>(args)...);
        }
        void Destruct() {
            DC_ASSERT(contructed_, "Object is not created");
            reinterpret_cast<T*>(&storage_)->~T();
            contructed_ = false;
        }
        T&& Move() {
            contructed_ = false;
            return std::move(*reinterpret_cast<T*>(&storage_));
        }

        T* operator->() {
            DC_ASSERT(contructed_, "Object is not created");
            return reinterpret_cast<T*>(&storage_);
        }
        const T* operator->() const {
            DC_ASSERT(contructed_, "Object is not created");
            return reinterpret_cast<const T*>(&storage_);
        }

        T* Get() {
            DC_ASSERT(contructed_, "Object is not created");
            return reinterpret_cast<T*>(&storage_);
        }
        const T* Get() const {
            DC_ASSERT(contructed_, "Object is not created");
            return reinterpret_cast<const T*>(&storage_);
        }
        bool Constructed() const { return contructed_; }

    private:
        alignas(T) std::byte storage_[sizeof(T)];
        bool contructed_ = false;
    };

}  // namespace NDummyConcurrency
