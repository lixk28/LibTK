#pragma once

#include "Weakable.h"

namespace TK {

template<typename T>
class WeakPtr {
    template<typename U>
    friend class Weakable;

public:
    WeakPtr() = default;
    ~WeakPtr() = default;

    WeakPtr(decltype(nullptr))
        : WeakPtr()
    {
    }

    WeakPtr(const WeakPtr& other)
        : m_flag(other.m_flag)
    {
    }

    WeakPtr(WeakPtr&& other)
        : m_flag(std::move(other.m_flag))
    {
    }

    template<typename U>
    WeakPtr(const WeakPtr<U>& other) requires(std::is_base_of<T, U>::value)
        : m_flag(other.m_flag)
    {
    }

    template<typename U>
    WeakPtr(WeakPtr<U>&& other) requires(std::is_base_of<T, U>::value)
        : m_flag(std::move(other.m_flag))
    {
    }

    WeakPtr(const RefPtr<T>& other) requires(std::is_base_of<Weakable<T>, T>::value)
        : WeakPtr(std::move(other->weak_from_this()))
    {
    }

    template<typename U>
    WeakPtr(const RefPtr<U>& other) requires(std::is_base_of<Weakable<U>, U>::value && std::is_base_of<T, U>::value)
        : WeakPtr(std::move(other->weak_from_this()))
    {
    }

    WeakPtr& operator=(decltype(nullptr))
    {
        m_flag = nullptr;
        return *this;
    }

    WeakPtr& operator=(const WeakPtr& other)
    {
        WeakPtr temp { other };
        swap(temp);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other)
    {
        WeakPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    template<typename U>
    WeakPtr& operator=(const WeakPtr<U>& other) requires(std::is_base_of<T, U>::value)
    {
        WeakPtr temp { other };
        swap(temp);
        return *this;
    }

    template<typename U>
    WeakPtr& operator=(WeakPtr<U>&& other) requires(std::is_base_of<T, U>::value)
    {
        WeakPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    WeakPtr& operator=(const RefPtr<T>& other) requires(std::is_base_of<Weakable<T>, T>::value)
    {
        WeakPtr temp { other };
        swap(temp);
        return *this;
    }

    template<typename U>
    WeakPtr& operator=(const RefPtr<U>& other) requires(std::is_base_of<Weakable<U>, U>::value && std::is_base_of<T, U>::value)
    {
        WeakPtr temp { other };
        swap(temp);
        return *this;
    }

    RefPtr<T> strong_ref() const
    {
        ASSERT(ptr());
        return RefPtr<T> { ptr() };
    }

    unsigned weak_cnt() const { return m_flag ? m_flag->ref_count() : 0; }

    T* ptr() const
    {
        if (m_flag)
            return m_flag->unsafe_ptr<T>();
        return nullptr;
    }

    // T* operator->() const { return ptr(); }
    // T& operator*() const { return *ptr(); }
    bool operator!() const { return !m_flag || !m_flag->is_valid(); }
    explicit operator bool() const { return m_flag && m_flag->is_valid(); }

    void swap(WeakPtr& other)
    {
        m_flag.swap(other.m_flag);
    }

private:
    explicit WeakPtr(const RefPtr<Internal::WeakFlag>& flag)
        : m_flag(flag)
    {
    }

    RefPtr<Internal::WeakFlag> m_flag { nullptr };
};

template<typename T>
ALWAYS_INLINE bool operator==(const WeakPtr<T>& a, decltype(nullptr))
{
    return a.ptr() == nullptr;
}

template<typename T>
ALWAYS_INLINE bool operator!=(const WeakPtr<T>& a, decltype(nullptr))
{
    return !(a == nullptr);
}

template<typename T>
ALWAYS_INLINE bool operator==(decltype(nullptr), const WeakPtr<T>& b)
{
    return b.ptr() == nullptr;
}

template<typename T>
ALWAYS_INLINE bool operator!=(decltype(nullptr), const WeakPtr<T>& b)
{
    return !(b == nullptr);
}

template<typename T>
ALWAYS_INLINE bool operator==(const WeakPtr<T>& a, const WeakPtr<T>& b)
{
    return a.ptr() == b.ptr();
}

template<typename T>
ALWAYS_INLINE bool operator!=(const WeakPtr<T>& a, const WeakPtr<T>& b)
{
    return !(a == b);
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const WeakPtr<T>& a, const WeakPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const WeakPtr<T>& a, const WeakPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return !(a == b);
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const WeakPtr<U>& a, const WeakPtr<T>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const WeakPtr<U>& a, const WeakPtr<T>& b) requires(std::is_base_of<T, U>::value)
{
    return !(a == b);
}

} // namespace TK
