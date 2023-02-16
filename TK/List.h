#pragma once

#include "Utility.h"
#include <cstdint>
#include <initializer_list>

namespace TK {

/* Doubly Linked List */
template<typename T>
class List {
private:
    /* List Node Base */
    struct ListNodeBase {
        ListNodeBase* m_prev { nullptr };
        ListNodeBase* m_next { nullptr };

        constexpr ListNodeBase() = default;

        /// @brief Hook this node right before the given node.
        /// @code
        /// +------+      +----------+
        /// |      |----->|          |
        /// | this |      | position |
        /// |      |<-----|          |
        /// +------+      +----------+
        /// @endcode
        void hook_before(ListNodeBase* node) noexcept
        {
            m_prev = node->m_prev;
            node->m_prev->m_next = this;

            m_next = node;
            node->m_prev = this;
        }

        /// @brief Hook this node right after the given node.
        /// @code
        /// +----------+      +------+
        /// |          |----->|      |
        /// | position |      | this |
        /// |          |<-----|      |
        /// +----------+      +------+
        /// @endcode
        void hook_after(ListNodeBase* node) noexcept
        {
            m_next = node->m_next;
            node->m_next->m_prev = this;

            m_prev = node;
            node->m_next = this;
        }

        /// @brief Unhooks this node from the linked list.
        /// @code
        /// +------+      +------+      +------+
        /// |      |----->|      |----->|      |
        /// | prev |      | this |      | next |
        /// |      |<-----|      |<-----|      |
        /// +------+      +------+      +------+
        /// @endcode
        void unhook() noexcept
        {
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
        }

        ~ListNodeBase() = default;
    };

    /* List Node */
    struct ListNode : public ListNodeBase {
        T m_value { };

        constexpr ListNode() noexcept
            : ListNodeBase()
        {
        }

        template<typename V>
        ListNode(V&& value)
            : ListNodeBase()
            , m_value (forward<V>(value))
        {
        }

        ~ListNode() = default;
    };

private:
    /* Base List Iterator */
    struct ListIteratorBase {
        constexpr ListIteratorBase() = default;

        constexpr explicit ListIteratorBase(ListNodeBase* node) noexcept
            : m_node (node) {
        }

        ~ListIteratorBase() = default;

        constexpr bool operator==(const ListIteratorBase& other) const noexcept { return m_node == other.m_node; }
        constexpr bool operator!=(const ListIteratorBase& other) const noexcept { return m_node != other.m_node; }

        T& operator*() noexcept { return static_cast<ListNode*>(m_node)->m_value; }
        const T& operator*() const noexcept { return static_cast<ListNode*>(m_node)->m_value; }

        T* operator->() noexcept { return &static_cast<ListNode*>(m_node)->m_value; }
        const T* operator->() const noexcept { return &static_cast<ListNode*>(m_node)->m_value; }

    protected:
        void increment() noexcept { m_node = m_node->m_next; }
        void decrement() noexcept { m_node = m_node->m_prev; }

    protected:
        ListNodeBase* m_node { nullptr };
    };

public:
    /* List Iterator */
    class ListIterator : public ListIteratorBase {
        friend List;

    public:
        constexpr ListIterator() noexcept
            : ListIteratorBase()
        {
        }


        ListIterator& operator++()
        {
            this->increment();
            return *this;
        }

        ListIterator operator++(int)
        {
            ListIterator it = *this;
            this->increment();
            return it;
        }

        ListIterator& operator--()
        {
            this->decrement();
            return *this;
        }

        ListIterator operator--(int)
        {
            ListIterator it = *this;
            this->decrement();
            return it;
        }

    private:
        constexpr explicit ListIterator(ListNodeBase* node) noexcept
            : ListIteratorBase(node)
        {
        }
    };

    /* List Reverse Iterator */
    class ListReverseIterator : public ListIteratorBase {
        friend List;

    public:
        constexpr ListReverseIterator() noexcept
            : ListIteratorBase()
        {
        }

        ListReverseIterator& operator++()
        {
            this->decrement();
            return *this;
        }

        ListReverseIterator operator++(int)
        {
            ListReverseIterator it = *this;
            this->decrement();
            return it;
        }

        ListReverseIterator& operator--()
        {
            this->increment();
            return *this;
        }

        ListReverseIterator operator--(int)
        {
            ListReverseIterator it = *this;
            this->increment();
            return it;
        }

    private:
        constexpr explicit ListReverseIterator(ListNodeBase* node) noexcept
            : ListIteratorBase(node)
        {
        }
    };

public:
    using ValueType = T;
    using Reference = T&;
    using ConstReference = const T&;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Iterator = ListIterator;
    using ConstIterator = const ListIterator;
    using ReverseIterator = ListReverseIterator;
    using ConstReverseIterator = const ListReverseIterator;

public:
    List() { connect_head_and_tail(); }

    ~List()
    {
        clear();
        delete m_head;
        delete m_tail;
    }

    List(const List& other)
    {
        connect_head_and_tail();
        for (const T& obj : other)
            push_back(obj);
    }

    List(List&& other) noexcept
    {
        connect_head_and_tail();
        if (!other.empty()) {
            m_head->m_next = other.m_head->m_next;
            other.m_head->m_next->m_prev = m_head;

            m_tail->m_prev = other.m_tail->m_prev;
            other.m_tail->m_prev->m_next = m_tail;

            other.connect_head_and_tail();

            m_size = other.m_size;
            other.m_size = 0;
        }
    }

    List(std::initializer_list<T> init_list)
    {
        connect_head_and_tail();
        for (const T& obj : init_list)
            push_back(obj);
    }

    List& operator=(const List& other)
    {
        if (this == &other)
            return *this;

        clear();

        for (const T& obj : other)
            push_back(obj);

        return *this;
    }

    List& operator=(List&& other) noexcept
    {
        if (this == &other)
            return *this;

        clear();

        if (!other.empty()) {
            m_head->m_next = other.m_head->m_next;
            other.m_head->m_next->m_prev = m_head;

            m_tail->m_prev = other.m_tail->m_prev;
            other.m_tail->m_prev->m_next = m_tail;

            other.connect_head_and_tail();

            m_size = other.m_size;
            other.m_size = 0;
        }

        return *this;
    }

    List& operator=(std::initializer_list<T> init_list)
    {
        clear();
        for (const T& obj : init_list)
            push_back(obj);
    }

    [[nodiscard]] bool empty() const noexcept { return m_head->m_next == m_tail; }

    [[nodiscard]] unsigned size() const noexcept { return m_size; }

    [[nodiscard]] T& front() { return static_cast<ListNode*>(m_head->m_next)->m_value; }
    [[nodiscard]] const T& front() const { return static_cast<ListNode*>(m_head->m_next)->m_value; }

    [[nodiscard]] T& back() { return static_cast<ListNode*>(m_tail->m_prev)->m_value; }
    [[nodiscard]] const T& back() const { return static_cast<ListNode*>(m_tail->m_prev)->m_value; }

    Iterator begin() noexcept { return Iterator(m_head->m_next); }
    Iterator end() noexcept { return Iterator(m_tail); }

    ConstIterator begin() const noexcept { return Iterator(m_head->m_next); }
    ConstIterator end() const noexcept { return Iterator(m_tail); }

    ConstIterator cbegin() const noexcept { return Iterator(m_head->next); }
    ConstIterator cend() const noexcept { return Iterator(m_tail); }

    ReverseIterator rbegin() noexcept { return ReverseIterator(m_tail->m_prev); }
    ReverseIterator rend() noexcept { return ReverseIterator(m_head); }

    ConstReverseIterator rbegin() const noexcept { return ReverseIterator(m_tail->m_prev); }
    ConstReverseIterator rend() const noexcept { return ReverseIterator(m_head); }

    ConstReverseIterator crbegin() const noexcept { return ReverseIterator(m_tail->m_prev); }
    ConstReverseIterator crend() const noexcept { return ReverseIterator(m_head); }

    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        ListNode* node = new ListNode(forward<Args>(args)...);
        node->hook_before(m_tail);
        m_size++;
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(move(value)); }

    void pop_back()
    {
        if (m_tail->m_prev != m_head) {
            ListNode* node = static_cast<ListNode*>(m_tail->m_prev);
            node->unhook();
            delete node;
            m_size--;
        }
    }

    template<typename... Args>
    void emplace_front(Args&&... args)
    {
        ListNode* node = new ListNode(forward<Args>(args)...);
        node->hook_after(m_head);
        m_size++;
    }

    void push_front(const T& value) { emplace_front(value); }
    void push_front(T&& value) { emplace_front(move(value)); }

    void pop_front()
    {
        if (m_head->m_next != m_tail) {
            ListNode* node = static_cast<ListNode*>(m_head->m_next);
            node->unhook();
            delete node;
            m_size--;
        }
    }

    void clear()
    {
        if (m_head && m_tail && m_head->m_next != m_tail) {
            ListNode* node = static_cast<ListNode*>(m_head->m_next);
            while (node != m_tail) {
                ListNode* curr = node;
                node = static_cast<ListNode*>(node->m_next);
                delete curr;
            }
            m_head->m_next = m_tail;
            m_tail->m_prev = m_head;
            m_size = 0;
        }
    }

    template<typename... Args>
    void emplace(ConstIterator pos, Args&&... args)
    {
        ListNode* node = new ListNode(forward<Args>(args)...);
        node->hook_before(pos.m_node);
        m_size++;
    }

    void insert(ConstIterator pos, const T& value) { emplace(pos, value); }
    void insert(ConstIterator pos, T&& value) { emplace(pos, move(value)); }

    /// @brief Remove the element at `pos`
    Iterator erase(ConstIterator pos)
    {
        if (pos == end())
            return end();

        Iterator it = Iterator(pos.m_node->m_next);
        ListNode* node = static_cast<ListNode*>(pos.m_node);
        node->unhook();
        delete node;

        return it;
    }

    /// @brief Remove the elements in the range `[first, last)`
    Iterator erase(ConstIterator first, ConstIterator last)
    {
        if (first == last)
            return last;

        auto it = first;
        for (auto it = first; it != last; )
            it = erase(it);

        return it;
    }

    void swap(List& other) noexcept
    {
        swap(m_head, other.m_head);
        swap(m_tail, other.m_tail);
        swap(m_size, other.m_size);
    }

    // TODO: Implement us TnT
    /// @brief Merge two sorted lists into one, the lists should be be sorted into ascending order
    void merge(List& other)
    {
    }

    void merge(List&& other)
    {
    }

    template<typename Compare>
    void merge(List& other, Compare comp)
    {
    }

    template<typename Compare>
    void merge(List&& other, Compare comp)
    {
    }

    void reverse() noexcept
    {
    }

    void remove(const T& value)
    {
        for (auto it = begin(); it != end(); ) {
            if (*it == value)
                it = erase(it);
            else
                it++;
        }
    }

    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate p)
    {
        for (auto it = begin(); it != end(); ) {
            if (p(*it))
                it = erase(it);
            else
                it++;
        }
    }

    // TODO: Implement insertion sort and merge sort
    void sort()
    {
    }

    template<typename Compare>
    void sort(Compare comp)
    {
    }

private:
    void connect_head_and_tail()
    {
        m_head->m_next = m_tail;
        m_tail->m_prev = m_head;
    }

private:
    ListNodeBase* m_head { new ListNodeBase };
    ListNodeBase* m_tail { new ListNodeBase };
    unsigned m_size { 0 };
};

template<typename T>
bool operator==(const List<T>& lhs, const List<T>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    for (auto it1 = lhs.begin(), it2 = rhs.begin(); it1 != lhs.end() && it2 != rhs.end(); it1++, it2++) {
        if (!(*it1 == *it2))
            return false;
    }

    return true;
}

}

using TK::List;
