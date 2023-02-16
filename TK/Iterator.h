#pragma once

#include <cstddef>

namespace ToolKit {

/* Sequential Iterator for containers owning continuous memory */
template<typename U, typename T>
class Iterator {
public:
    using Container      = U;
    using ValueType      = T;
    using Reference      = T&;
    using ConstReference = const T&;
    using Pointer        = T*;
    using ConstPointer   = const T*;
    using DifferenceType = std::ptrdiff_t;

public:
    friend Container;

public:
    constexpr Iterator() = default;

    constexpr Iterator(const Iterator& other)
        : m_ptr (other.m_ptr)
    {
    }

    ~Iterator() = default;

    constexpr Iterator& operator=(const Iterator& other)
    {
        m_ptr = other.m_ptr;
        return *this;
    }

    constexpr Iterator& operator++()
    {
        m_ptr++;
        return *this;
    }

    constexpr Iterator operator++(int)
    {
        Iterator it = *this;
        ++(*this);
        return it;
    }

    constexpr Iterator& operator--()
    {
        m_ptr--;
        return *this;
    }

    constexpr Iterator operator--(int)
    {
        Iterator it = *this;
        --(*this);
        return it;
    }

    [[nodiscard]] constexpr Iterator operator+(DifferenceType offset) const { return Iterator {m_ptr + offset}; }
    [[nodiscard]] constexpr Iterator operator-(DifferenceType offset) const { return Iterator {m_ptr - offset}; }
    [[nodiscard]] constexpr DifferenceType operator-(const Iterator& other) const { return m_ptr - other.m_ptr; }

    constexpr Iterator& operator+=(DifferenceType offset)
    {
        m_ptr += offset;
        return *this;
    }

    constexpr Iterator& operator-=(DifferenceType offset)
    {
        m_ptr -= offset;
        return *this;
    }

    [[nodiscard]] constexpr DifferenceType operator-=(const Iterator& other)
    {
        return m_ptr - other.m_ptr;
    }

    [[nodiscard]] constexpr bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
    [[nodiscard]] constexpr bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
    [[nodiscard]] constexpr bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
    [[nodiscard]] constexpr bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
    [[nodiscard]] constexpr bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }
    [[nodiscard]] constexpr bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }

    [[nodiscard]] constexpr Reference      operator*()       { return *m_ptr; }
    [[nodiscard]] constexpr ConstReference operator*() const { return *m_ptr; }

    [[nodiscard]] constexpr Reference      operator[](DifferenceType offset)       { return m_ptr[offset]; }
    [[nodiscard]] constexpr ConstReference operator[](DifferenceType offset) const { return m_ptr[offset]; }

    [[nodiscard]] constexpr Pointer      operator->()       { return m_ptr; }
    [[nodiscard]] constexpr ConstPointer operator->() const { return m_ptr; }

private:
    constexpr explicit Iterator(ValueType* ptr)
        : m_ptr (ptr)
    {
    }

private:
    Pointer m_ptr { nullptr };
};

}

using ToolKit::Iterator;
