#pragma once

#include "Iterator.h"
#include "Utility.h"
#include <new>
#include <initializer_list>

namespace ToolKit {

template <typename T>
class Vector {
public:
    using SizeType       = unsigned;
    using ValueType      = T;
    using Reference      = ValueType&;
    using ConstReference = const ValueType&;
    using Pointer        = ValueType*;
    using ConstPointer   = const ValueType*;
    using VectorIterator = Iterator<Vector<ValueType>, ValueType>;

public:
    Vector() = default;

    constexpr Vector(const Vector& other)
        : m_data (static_cast<T*>(::operator new(sizeof(T) * other.m_capacity)))
        , m_size (other.m_size)
        , m_capacity (other.m_capacity)
    {
        for (unsigned i = 0; i < m_size; i++)
            new(&m_data[i]) T(other.m_data[i]);
    }

    constexpr Vector(Vector&& other) noexcept
        : m_data (other.m_data)
        , m_size (other.m_size)
        , m_capacity (other.m_capacity)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    constexpr Vector(std::initializer_list<T> init_list)
    {
        reserve(init_list.size());
        for (auto& obj: init_list)
            push_back(obj);
    }

    constexpr explicit Vector(unsigned size)
    {
        m_size = size;
        m_capacity = size;
        m_data = static_cast<T*>(::operator new(sizeof(T) * m_capacity));
        for (unsigned i = 0; i < m_size; i++)
            new(&m_data[i]) T();
    }

    constexpr Vector(unsigned size, const T& value)
    {
        m_size = size;
        m_capacity = size;
        m_data = static_cast<T*>(::operator new(sizeof(T) * m_capacity));
        for (unsigned i = 0; i < m_size; i++)
            new(&m_data[i]) T(value);
    }

    constexpr Vector(const VectorIterator begin, const VectorIterator end)
    {
        m_size = end - begin;
        m_capacity = m_size;
        m_data = static_cast<T*>(::operator new(sizeof(T) * m_capacity));
        for (unsigned i = 0; i < m_size; i++)
            new(&m_data[i]) T(*(begin + i));
    }

    ~Vector()
    {
        if (m_data) {
            for (unsigned i = 0; i < m_size; i++)
                m_data[i].~T();
            ::operator delete(m_data);
        }
        m_size = 0;
        m_capacity = 0;
    }

    constexpr Vector& operator=(const Vector& other)
    {
       if (&other == this)
           return *this;

       if (m_data) {
           for (unsigned i = 0; i < m_size; i++)
               m_data[i].~T();
       }

       /* if the LHS vector cannot contain the RHS vector, reallocate enough memory */
       if (m_capacity < other.m_size) {
           ::operator delete(m_data);
           m_data = static_cast<T*>(::operator new(sizeof(T) * other.m_size));
           m_capacity = other.m_size;
       }

       for (unsigned i = 0; i < other.m_size; i++)
           new(&m_data[i]) T(other.m_data[i]);

       m_size = other.m_size;

       return *this;
    }

    constexpr Vector& operator=(Vector&& other) noexcept
    {
        if (&other == this)
            return *this;

        if (m_data) {
            for (unsigned i = 0; i < m_size; i++)
                m_data[i].~T();
            ::operator delete(m_data);
        }

        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;

        return *this;
    }

    constexpr Vector& operator=(std::initializer_list<T> init_list)
    {
        if (m_data) {
            for (unsigned i = 0; i < m_size; i++)
                m_data[i].~T();
        }

        if (m_capacity < init_list.size()) {
            ::operator delete(m_data);
            m_data = static_cast<T*>(::operator new(sizeof(T) * init_list.size()));
            m_capacity = init_list.size();
        }

        for(auto& obj: init_list)
            push_back(obj);

        return *this;
    }

    bool operator==(const Vector& other) = delete;
    bool operator!=(const Vector& other) = delete;
    bool operator>(const Vector& other) = delete;
    bool operator<(const Vector& other) = delete;
    bool operator>=(const Vector& other) = delete;
    bool operator<=(const Vector& other) = delete;

    [[nodiscard]] constexpr T& operator[](unsigned index) noexcept { return m_data[index]; }
    [[nodiscard]] constexpr const T& operator[](unsigned index) const noexcept { return m_data[index]; }

    [[nodiscard]] constexpr unsigned capacity() const noexcept { return m_capacity; }
    [[nodiscard]] constexpr unsigned size() const noexcept { return m_size; }
    [[nodiscard]] constexpr bool empty() const noexcept { return m_size == 0; }

    [[nodiscard]] constexpr VectorIterator begin() noexcept { return VectorIterator(m_data); }
    [[nodiscard]] constexpr VectorIterator end() noexcept { return VectorIterator(m_data + m_size); }
    [[nodiscard]] constexpr const VectorIterator cbegin() const noexcept { return VectorIterator(m_data); }
    [[nodiscard]] constexpr const VectorIterator cend() const noexcept { return VectorIterator(m_data + m_size); }

    [[nodiscard]] constexpr VectorIterator rbegin() noexcept { }
    [[nodiscard]] constexpr VectorIterator rend() noexcept { }
    [[nodiscard]] constexpr const VectorIterator crbegin() const noexcept { }
    [[nodiscard]] constexpr const VectorIterator crend() const noexcept { }

    constexpr T& at(unsigned idx)
    {
        // TODO: Cup internal out of range error
        if (idx >= m_size)
            ;

        return m_data[idx];
    }

    constexpr const T& at(unsigned idx) const
    {
        // TODO: Cup internal out of range error
        if (idx >= m_size)
            ;

        return m_data[idx];
    }

    [[nodiscard]] constexpr T& front() { return m_data[0]; }
    [[nodiscard]] constexpr const T& front() const { return m_data[0]; }

    [[nodiscard]] constexpr T& back() { return m_data[m_size - 1]; }
    [[nodiscard]] constexpr const T& back() const { return m_data[m_size - 1]; }

    [[nodiscard]] constexpr T* data() noexcept { return m_data; }
    [[nodiscard]] constexpr const T* data() const noexcept { return m_data; }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/insert (1)
    constexpr void insert(const VectorIterator pos, const T& value)
    {
        emplace(pos, value);
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/insert (2)
    constexpr void insert(const VectorIterator pos, T&& value)
    {
        emplace(pos, move(value));
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/insert (3)
    constexpr void insert(const VectorIterator pos, unsigned count, const T& value)
    {
        for (unsigned i = 0; i < count; i++)
            emplace(pos, value);
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/insert (4)
    constexpr void insert(const VectorIterator pos, const VectorIterator first, const VectorIterator last)
    {
        for (auto it = first; it != last; it++)
            emplace(pos, *it);
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/insert (5)
    constexpr void insert(const VectorIterator pos, std::initializer_list<T> init_list)
    {
        for (auto it = init_list.begin(); it != init_list.end(); it++)
            emplace(pos, *it);
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/emplace
    template<typename... Args>
    constexpr void emplace(const VectorIterator pos, Args&&... args)
    {
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/erase (1)
    // erase pos
    constexpr void erase(const VectorIterator pos)
    {
    }

    // @ref: https://en.cppreference.com/w/cpp/container/vector/erase (2)
    // erase [first, last)
    constexpr void erase(const VectorIterator first, const VectorIterator last)
    {
    }

    constexpr void swap(Vector& other) noexcept
    {
        swap(m_data, other.m_data);
        swap(m_size, other.m_size);
        swap(m_capacity, other.m_capacity);
    }

    template<typename F>
    void for_each(F func)
    {
        for (auto it = begin(); it != end(); it++)
            func(*it);
    }

    template<typename... Args>
    constexpr void emplace_back(Args&&... args)
    {
        if (m_size >= m_capacity)
            realloc(new_capacity());

        new(&m_data[m_size++]) T(forward<Args>(args)...);
    }

    constexpr void push_back(const T& value) { emplace_back(value); }
    constexpr void push_back(T&& value) { emplace_back(move(value)); }

    constexpr void pop_back()
    {
        if (m_size > 0)
            m_data[--m_size].~T();
    }

    constexpr void reserve(unsigned new_capacity)
    {
        if (new_capacity > m_capacity)
            realloc(new_capacity);
    }

    constexpr void resize(unsigned new_size)
    {
        if (m_size > new_size) {
            for (unsigned i = new_size; i < m_size; i++)
                m_data[i].~T();
        }

        if (m_size < new_size) {
            for (unsigned i = 0; i < new_size - m_size; i++)
                push_back(T());
        }
    }

    constexpr void resize(unsigned new_size, const T& value)
    {
    }

    constexpr void clear() noexcept
    {
        if (m_data) {
            for (unsigned i = 0; i < m_size; i++)
                m_data[i].~T();
            m_size = 0;
        }
    }

private:
    constexpr unsigned new_capacity()
    {
        return m_capacity == 0 ? 1 : 2 * m_capacity;
    }

    constexpr void realloc(unsigned new_capacity)
    {
        T* new_data = static_cast<T*>(::operator new(sizeof(T) * new_capacity));

        if (new_capacity < m_size)
            m_size = new_capacity;

        if (m_data) {
            for (unsigned i = 0; i < m_size; i++)
                new(&new_data[i]) T(move(m_data[i]));

            for (unsigned i = 0; i < m_size; i++)
                m_data[i].~T();
            ::operator delete(m_data);
        }

        m_data = new_data;
        m_capacity = new_capacity;
    }

private:
    T* m_data { nullptr };
    unsigned m_size { 0 };
    unsigned m_capacity { 0 };
};

}

using ToolKit::Vector;
