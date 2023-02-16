#pragma once

#include "List.h"
#include "Utility.h"

namespace TK {

/* Linked Double-ended Queue */
template<typename T, typename Container = List<T>>
class Deque {
public:
    using ValueType = T;
    using Reference = T&;
    using ConstReference = const T&;
    using Pointer = T*;
    using ConstPointer = const T*;
    using SizeType = unsigned;

public:
    Deque() = default;
    ~Deque() = default;

    Deque(const Deque& other)
        : m_deque (other.m_deque)
    {
    }

    Deque(Deque&& other) noexcept
        : m_deque (TK::move(other.m_deque))
    {
    }

    Deque& operator=(const Deque& other) { m_deque = other.m_deque; }
    Deque& operator=(Deque&& other) noexcept { m_deque = TK::move(other.m_deque); }

    T& front() noexcept { return m_deque.front(); }
    const T& front() const noexcept { return m_deque.front(); }

    T& back() noexcept { return m_deque.back(); }
    const T& back() const noexcept { return m_deque.back(); }

    [[nodiscard]] bool empty() const { return m_deque.empty(); }
    unsigned size() const { return m_deque.size(); }

    void clear() noexcept { m_deque.clear(); }

    template<typename... Args>
    void emplace_back(Args... args) { m_deque.emplace_back(TK::forward<Args>(args)...); }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(TK::move(value)); }

    void pop_back() { m_deque.pop_back(); }

    template<typename... Args>
    void emplace_front(Args... args) { m_deque.emplace_front(TK::forward<Args>(args)...); }

    void push_front(const T& value) { emplace_front(value); }
    void push_front(T&& value) { emplace_front(TK::move(value)); }

    void pop_front() { m_deque.pop_front(); }

    // TODO: Implement us
    void emplace() {}

    void insert() {}

    void erase() {}

private:
    Container m_deque { };
};

// TODO: Implement us
template<typename T>
bool operator==(const Deque<T>& lhs, const Deque<T>& rhs)
{
}

template<typename T>
void swap(Deque<T>& lhs, Deque<T>& rhs) noexcept
{
}

}

using TK::Deque;
