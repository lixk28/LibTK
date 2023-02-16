#pragma once

#include "Assertions.h"
#include "Definitions.h"
#include <type_traits>
#include <utility>

namespace TK {

// Non-Null and Unthread-Safe Reference Counting Pointer
// `T` must inherit `RefCounted<T>` publicly to use `Ref<T>`

// A `Ref<T>` may share the resource it holds with `RefPtr<T>`
// But `RefPtr<T>` may share its resource only when the pointer it holds is not `nullptr`

// TODO:
// Functions and operators converting `RefPtr<T>` to `Ref<T>`, requires `release_nonnull()` in `RefPtr<T>`

template<typename T>
class [[nodiscard]] Ref {
public:
    // `Ref<T>` cannot be default constructed
    Ref() = delete;

    ALWAYS_INLINE ~Ref()
    {
        if (m_ptr)
            m_ptr->deref();
        m_ptr = nullptr;
    }

    ALWAYS_INLINE Ref(const T& object)
        : m_ptr (const_cast<T*>(&object))
    {
        m_ptr->ref();
    }

    ALWAYS_INLINE Ref(const Ref& other)
        : m_ptr (other.m_ptr)
    {
        m_ptr->ref();
    }

    ALWAYS_INLINE Ref(Ref&& other) noexcept
        : m_ptr (other.release())
    {
        ASSERT(m_ptr);
    }

    template<typename U>
    ALWAYS_INLINE Ref(const U& object) requires(std::is_convertible<U*, T*>::value)
        : m_ptr (const_cast<T*>(static_cast<const T*>(&object)))
    {
        m_ptr->ref();
    }

    template<typename U>
    ALWAYS_INLINE Ref(const Ref<U>& other) requires(std::is_convertible<U*, T*>::value)
        : m_ptr (const_cast<T*>(static_cast<const T*>(other.m_ptr)))
    {
        m_ptr->ref();
    }

    template<typename U>
    ALWAYS_INLINE Ref(Ref<U>&& other) noexcept requires(std::is_convertible<U*, T*>::value)
        : m_ptr (other.release())
    {
        ASSERT(m_ptr);
    }

    ALWAYS_INLINE Ref& operator=(const Ref& other)
    {
        Ref temp { other };
        swap(temp);
        return *this;
    }

    ALWAYS_INLINE Ref& operator=(Ref&& other) noexcept
    {
        Ref temp { std::move(other) };
        swap(temp);
        return *this;
    }

    template<typename U>
    ALWAYS_INLINE Ref& operator=(const Ref<U>& other) requires(std::is_convertible<U*, T*>::value)
    {
        Ref temp { other };
        swap(temp);
        return *this;
    }

    template<typename U>
    ALWAYS_INLINE Ref& operator=(Ref<U>&& other) noexcept requires(std::is_convertible<U*, T*>::value)
    {
        Ref temp { std::move(other) };
        swap(temp);
        return *this;
    }

    Ref& operator=(decltype(nullptr)) = delete;

    // `Ref<T>` can be implicitly converted to `T&`, `const T&`, that's awesome!
    ALWAYS_INLINE operator T&() { return *as_nonnull_ptr(); }
    ALWAYS_INLINE operator const T&() const { return *as_nonnull_ptr(); }
    // NOTE: Maybe we don't want `Ref<T>` to behave like a pointer
    // ALWAYS_INLINE operator T*() { return as_nonnull_ptr(); }
    // ALWAYS_INLINE operator const T*() const { return as_nonnull_ptr(); }

    // Get the pointer it holds
    ALWAYS_INLINE T* ptr() const { return as_nonnull_ptr(); }

    // NOTE: Maybe we don't want `Ref<T>` to behave like a pointer
    ALWAYS_INLINE T* operator->() { return as_nonnull_ptr(); }
    ALWAYS_INLINE const T* operator->() const { return as_nonnull_ptr(); }
    ALWAYS_INLINE T& operator*() { return *as_nonnull_ptr(); }
    ALWAYS_INLINE const T& operator*() const { return *as_nonnull_ptr(); }

    // Because `Ref<T>` is non-null, there is no reason to check if it's null
    operator bool() const = delete;
    bool operator!() const = delete;

    // Release the pointer it holds with reference count unchanged
    [[nodiscard]] T* release()
    {
        return std::exchange(m_ptr, nullptr);
    }

    void swap(Ref& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }

    template<typename U>
    void swap(Ref<U>& other) noexcept requires(std::is_convertible<U*, T*>::value)
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

template<typename T>
void swap(Ref<T>& a, Ref<T>& b) noexcept
{
    a.swap(b);
}

template<typename T, typename U>
void swap(Ref<T>& a, Ref<U>& b) noexcept requires(std::is_convertible<U*, T*>::value)
{
    a.swap(b);
}

template<typename T>
ALWAYS_INLINE bool operator==(const Ref<T>& a, const Ref<T>& b)
{
    return a.ptr() == b.ptr();
}

template<typename T>
ALWAYS_INLINE bool operator!=(const Ref<T>& a, const Ref<T>& b)
{
    return a.ptr() != b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const Ref<T>& a, const Ref<U>& b) requires(std::is_convertible<U*, T*>::value)
{
    return a.ptr() == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const Ref<T>& a, const Ref<U>& b) requires(std::is_convertible<U*, T*>::value)
{
    return a.ptr() != b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const Ref<T>& a, U* b) requires(std::is_convertible<U*, T*>::value)
{
    return a.ptr() == b;
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const Ref<T>& a, U* b) requires(std::is_convertible<U*, T*>::value)
{
    return a.ptr() != b;
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(T* a, const Ref<U>& b) requires(std::is_convertible<U*, T*>::value)
{
    return a == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(T* a, const Ref<U>& b) requires(std::is_convertible<U*, T*>::value)
{
    return a != b.ptr();
}

template<typename T, typename... Args> requires(std::is_constructible<T, Args...>::value)
ALWAYS_INLINE Ref<T> make_ref(Args&&... args)
{
    T* ptr = new T(std::forward<Args>(args)...);
    ASSERT(ptr);
    return *ptr;
}

}
