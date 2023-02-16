#pragma once

#include "Vector.h"
#include "Utility.h"

namespace TK {

/* Continuous Queue with Vector as underlying container */
template<typename T, typename Container = Vector<T>>
class Queue {
public:
    using ContainerType = Container;
    using ValueType = T;
    using Reference = T&;
    using ConstReference = const T&;
    using Pointer = T*;
    using ConstPointer = const T*;
    using SizeType = unsigned;

public:
    Queue() = default;
    ~Queue() = default;

    Queue(const Queue& other)
        : m_container (other.m_container)
    {
    }

    Queue(Queue&& other) noexcept
        : m_container (move(other.m_container))
    {
    }

    Queue& operator=(const Queue& other) { m_container = other.m_container; }

    Queue& operator=(Queue&& other) noexcept { m_container = move(other.m_container); }

    T& front() noexcept { return m_container.front(); }
    const T& front() const noexcept { return m_container.front(); }

    T& back() noexcept { return m_container.back(); }
    const T& back() const noexcept { return m_container.back(); }

    [[nodiscard]] bool empty() const { return m_container.empty(); }
    unsigned size() const { return m_container.size(); }

    void push(const T& value) { m_container.push_back(value); }
    void push(T&& value) { m_container.push_back(move(value)); }

    template<typename... Args>
    void emplace(Args... args) { m_container.emplace_back(forward<Args>(args)...); }

    void pop() { m_container.pop_front(); }

    void swap(Queue& other) noexcept
    {
        using TK::swap;
        swap(m_container, other.m_container);
    }

private:
    ContainerType m_container { };
};

} // namespace TK

using TK::Queue;
