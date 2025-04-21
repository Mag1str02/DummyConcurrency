#pragma once

#include <DummyConcurrency/Utils/Assert.hpp>

#include <stddef.h>

namespace DummyConcurrency {

    struct IntrusiveForwardListNodeDefaultTag {};

    template <typename T, typename Tag = IntrusiveForwardListNodeDefaultTag>
    struct IntrusiveForwardListNode {
        // Shortcut
        using Node = IntrusiveForwardListNode;

        Node* next_ = nullptr;

        Node* Next() noexcept { return next_; }

        void SetNext(Node* node) noexcept { next_ = node; }

        void ResetNext() noexcept { SetNext(nullptr); }

        void LinkBefore(Node* node) noexcept { SetNext(node); }

        T* AsItem() noexcept { return static_cast<T*>(this); }
    };

    template <typename T, typename Tag = IntrusiveForwardListNodeDefaultTag> class IntrusiveForwardList {
        // Node type
        using Node = IntrusiveForwardListNode<T, Tag>;

        using List = IntrusiveForwardList<T, Tag>;

    public:
        IntrusiveForwardList() = default;

        // Non-copyable
        IntrusiveForwardList(const List&)            = delete;
        IntrusiveForwardList& operator=(const List&) = delete;

        // Move-constructable
        IntrusiveForwardList(List&& that) {
            head_ = that.head_;
            tail_ = that.tail_;
            size_ = that.size_;
            that.Reset();
        }

        IntrusiveForwardList& operator=(List&& that) = delete;

        void PushBack(Node* node) noexcept {
            ++size_;

            node->next_ = nullptr;

            if (IsEmpty()) {
                head_ = tail_ = node;
            } else {
                tail_->next_ = node;
                tail_        = node;
            }
        }

        // O(1)
        void Append(List& that) noexcept {
            if (that.IsEmpty()) {
                return;
            }

            if (IsEmpty()) {
                head_ = that.head_;
                tail_ = that.tail_;
                size_ = that.size_;
            } else {
                tail_->next_ = that.head_;
                tail_        = that.tail_;
                size_ += that.size_;
            }

            that.Reset();
        }

        void PushFront(Node* node) noexcept {
            ++size_;
            if (IsEmpty()) {
                head_ = tail_ = node;
            } else {
                node->next_ = head_;
                head_       = node;
            }
        }

        T* PopFront() noexcept {
            if (IsEmpty()) {
                return nullptr;
            }

            --size_;
            Node* front = head_;

            if (head_ == tail_) {
                head_ = tail_ = nullptr;
            } else {
                head_ = head_->next_;
            }

            front->next_ = nullptr;

            return front->AsItem();
        }

        bool IsEmpty() const noexcept { return head_ == nullptr; }

        bool NonEmpty() const noexcept { return !IsEmpty(); }

        bool HasItems() const noexcept { return !IsEmpty(); }

        // O(1) worst case
        size_t Size() const noexcept { return size_; }

        // TODO: Iterators

        template <typename F> void ForEach(F&& handler) {
            Node* iter = head_;
            while (iter != nullptr) {
                handler(iter->AsItem());
                iter = iter->next_;
            }
        }

        ~IntrusiveForwardList() { DC_ASSERT(IsEmpty(), "List is not empty"); }

    private:
        void Reset() noexcept {
            head_ = nullptr;
            tail_ = nullptr;
            size_ = 0;
        }

    private:
        Node*  head_ = nullptr;
        Node*  tail_ = nullptr;
        size_t size_ = 0;
    };

}  // namespace DummyConcurrency
