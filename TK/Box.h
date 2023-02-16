#pragma once

#include "Assertions.h"
#include "Definitions.h"
#include "NonCopyable.h"
#include <utility>

namespace TK {

// Non-Null Mutable Owning Pointer
// Stole From Rust :^)

// TODO:
// Functions and operators converting `OwnPtr<T>` to `Box<T>`, requires 'release_nonnull()` in `OwnPtr<T>`

template<typename T>
class [[nodiscard]] Box {
    TK_MAKE_NONCOPYABLE(Box)

public:
    // `Box<T>` cannot be default constructed
    Box() = delete;

    ALWAYS_INLINE ~Box()
    {
        delete m_ptr;
    }

    Box(decltype(nullptr)) = delete;
    Box(const T* ptr) = delete;
    template<typename U> Box(const U* ptr) = delete;

    ALWAYS_INLINE explicit Box(const T& object) noexcept
        : m_ptr(&object)
    {
    }

    ALWAYS_INLINE Box(Box&& other) noexcept
        : m_ptr(other.release())
    {
    }

    template<typename U>
    ALWAYS_INLINE Box(Box<U>&& other) noexcept requires(std::is_convertible<U*, T*>::value)
        : m_ptr(static_cast<T*>(other.release()))
    {
    }

    ALWAYS_INLINE Box& operator=(Box&& other) noexcept
    {
        Box temp { std::move(other) };
        swap(temp);
        return *this;
    }

    template<typename U>
    ALWAYS_INLINE Box& operator=(Box<U>&& other) noexcept requires(std::is_convertible<U*, T*>::value)
    {
        Box temp { std::move(other) };
        swap(temp);
        return *this;
    }

    Box& operator=(decltype(nullptr)) = delete;
    Box& operator=(const T* ptr) = delete;
    template<typename U> Box& operator=(const U* ptr) = delete;

    ALWAYS_INLINE T* ptr() const noexcept { return as_nonnull_ptr(); }

    ALWAYS_INLINE T* operator->() noexcept { return as_nonnull_ptr(); }
    ALWAYS_INLINE const T* operator->() const noexcept { return as_nonnull_ptr(); }

    ALWAYS_INLINE T& operator*() noexcept { return *as_nonnull_ptr(); }
    ALWAYS_INLINE const T& operator*() const noexcept { return *as_nonnull_ptr(); }

    ALWAYS_INLINE operator T&() { return *as_nonnull_ptr(); }
    ALWAYS_INLINE operator const T&() const { return *as_nonnull_ptr(); }
    ALWAYS_INLINE operator T*() { return as_nonnull_ptr(); }
    ALWAYS_INLINE operator const T*() const { return as_nonnull_ptr(); }

    // A `Box<T>` is non-null
    bool operator!() const = delete;
    explicit operator bool() const = delete;

    [[nodiscard]] T* release()
    {
        return std::exchange(m_ptr, nullptr);
    }

    void swap(Box& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }

    template<typename U>
    void swap(Box<U>& other) noexcept requires(std::is_convertible<U*, T*>::value)
    {
        std::swap(m_ptr, other.m_ptr);
    }

private:
    ALWAYS_INLINE T* as_nonnull_ptr() const
    {
        ASSERT(m_ptr);
        return m_ptr;
    }

private:
    T* m_ptr { nullptr };
};

template<typename T, typename... Args> requires(std::is_constructible<T, Args...>::value)
ALWAYS_INLINE Box<T> make_box(Args&&... args)
{
    T* ptr = new T(std::forward<Args>(args)...);
    ASSERT(ptr);
    return *ptr;
}


}
