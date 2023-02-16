#pragma once

#include "Vector.h"
#include "Utility.h"

namespace TK {

/* Continuous Stack with Vector as its underlying container */
template<typename T, typename Container = Vector<T>>
class Stack {
public:
    using ContainerType = Container;
    using SizeType = unsigned;
    using ValueType = T;
    using Reference = T&;
    using ConstReference = const T&;
    using Pointer = T*;
    using ConstPointer = const T*;

public:
    Stack() = default;
    ~Stack() = default;

    Stack(const Stack& other)
        : m_container (other.m_container)
    {
    }

    Stack(Stack&& other)
        : m_container (TK::move(other.m_container))
    {
    }

    Stack& operator=(const Stack& other) { m_container = other.m_container; }

    Stack& operator=(Stack&& other) { m_container = TK::move(other.m_size); }

    T& top() { return m_container.back(); }
    const T& top() const { return m_container.back(); }

    [[nodiscard]] bool empty() const { return m_container.empty(); }

    unsigned size() const { return m_container.size(); }

    void push(const T& value) { m_container.push_back(value); }
    void push(T&& value) { m_container.push_back(TK::move(value)); }

    template<typename... Args>
    void emplace(Args... args) { m_container.emplace_back(TK::forward<Args>(args)...); }

    void pop() { m_container.pop_back(); }

    void swap(Stack& other) noexcept
    {
        using TK::swap;
        swap(m_container, other.m_container);
    }

private:
    ContainerType m_container { };
};

} // namespace TK

using TK::Stack;
