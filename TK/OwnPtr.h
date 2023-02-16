#pragma once

#include "Box.h"
#include "NonCopyable.h"
#include "Definitions.h"
#include <type_traits>
#include <utility>

namespace TK {

template<typename T>
class [[nodiscard]] OwnPtr {
    TK_MAKE_NONCOPYABLE(OwnPtr)

public:
    ALWAYS_INLINE OwnPtr() = default;
    ALWAYS_INLINE ~OwnPtr() { clear(); }

    ALWAYS_INLINE OwnPtr(decltype(nullptr))
        : m_ptr(nullptr)
    {
    }

    ALWAYS_INLINE explicit OwnPtr(const T* ptr)
        : m_ptr(const_cast<T*>(ptr))
    {
    }

    template<typename U>
    ALWAYS_INLINE OwnPtr(const U* ptr) requires(std::is_convertible<U*, T*>::value)
        : m_ptr(const_cast<T*>(static_cast<const T*>(ptr)))
    {
    }

    ALWAYS_INLINE OwnPtr(OwnPtr&& other) noexcept
        : m_ptr(other.release())
    {
    }

    template<typename U>
    ALWAYS_INLINE OwnPtr(OwnPtr<U>&& other) noexcept requires(std::is_convertible<U*, T*>::value)
        : m_ptr(other.release())
    {
    }

    // Construction from other pointers is not allowed
    // template<typename U> OwnPtr(const Box<U>& other) = delete;

    OwnPtr& operator=(decltype(nullptr))
    {
        clear();
        return *this;
    }

    OwnPtr& operator=(const T* ptr) = delete;

    template<typename U>
    OwnPtr& operator=(const U* ptr) requires(std::is_convertible<U*, T*>::value)
    {
        OwnPtr temp { ptr };
        swap(temp);
        return *this;
    }

    OwnPtr& operator=(OwnPtr&& other) noexcept
    {
        OwnPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    template<typename U>
    OwnPtr& operator=(OwnPtr<U>&& other) noexcept requires(std::is_convertible<U*, T*>::value)
    {
        OwnPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    // Assignment from other pointers is not allowed
    // template<typename U> OwnPtr& operator=(const Box<U>& other) = delete;

    // An object should not owned by two `OwnPtr`, thus comparation between `OwnPtr` is meaningless
    bool operator==(const OwnPtr& other) = delete;
    bool operator!=(const OwnPtr& other) = delete;

    ALWAYS_INLINE T* operator->() const { return as_nonnull_ptr(); }
    ALWAYS_INLINE T& operator*() const { return *as_nonnull_ptr(); }
    ALWAYS_INLINE operator T*() const { return as_ptr(); }
    ALWAYS_INLINE bool operator!() const { return is_null(); }
    ALWAYS_INLINE explicit operator bool() const { return !is_null(); }

    ALWAYS_INLINE T* ptr() const { return as_ptr(); }

    void swap(OwnPtr& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }

    template<typename U>
    void swap(OwnPtr<U>& other) noexcept requires(std::is_convertible<U*, T*>::value)
    {
        std::swap(m_ptr, other.m_ptr);
    }

    // Release the raw pointer it owns, and the pointer owned becomes `nullptr`
    [[nodiscard]] T* release()
    {
        return std::exchange(m_ptr, nullptr);
    }

private:
    ALWAYS_INLINE bool is_null() const { return !m_ptr; }
    ALWAYS_INLINE T* as_ptr() const { return m_ptr; }
    ALWAYS_INLINE T* as_nonnull_ptr() const
    {
        ASSERT(m_ptr);
        return m_ptr;
    }

    ALWAYS_INLINE void clear()
    {
        delete m_ptr;
        m_ptr = nullptr;
    }

private:
    T* m_ptr { nullptr };
};

template<typename T>
void swap(OwnPtr<T>& a, OwnPtr<T>& b) noexcept
{
    a.swap(b);
}

template<typename T, typename U>
void swap(OwnPtr<T>& a, OwnPtr<U>& b) noexcept requires(std::is_convertible<U*, T*>::value)
{
    a.swap(b);
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const OwnPtr<T>& a, U* b) requires(std::is_convertible<U*, T*>::value)
{
    return a.ptr() == b;
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const OwnPtr<T>& a, U* b) requires(std::is_convertible<U*, T*>::value)
{
    return a.ptr() != b;
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(T* a, const OwnPtr<U>& b) requires(std::is_convertible<U*, T*>::value)
{
    return a == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(T* a, const OwnPtr<U>& b) requires(std::is_convertible<U*, T*>::value)
{
    return a != b.ptr();
}

template<typename T>
ALWAYS_INLINE bool operator==(const OwnPtr<T>& a, decltype(nullptr))
{
    return a.ptr() == nullptr;
}

template<typename T>
ALWAYS_INLINE bool operator!=(const OwnPtr<T>& a, decltype(nullptr))
{
    return a.ptr() != nullptr;
}

template<typename T>
ALWAYS_INLINE bool operator==(decltype(nullptr), const OwnPtr<T>& b)
{
    return nullptr == b.ptr();
}

template<typename T>
ALWAYS_INLINE bool operator!=(decltype(nullptr), const OwnPtr<T>& b)
{
    return nullptr != b.ptr();
}

template<typename T, typename... Args> requires(std::is_constructible<T, Args...>::value)
ALWAYS_INLINE OwnPtr<T> make_own(Args... args)
{
    return new T(std::forward<Args>(args)...);
}

}
