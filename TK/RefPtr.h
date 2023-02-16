#pragma once

#include "Ref.h"
#include "Definitions.h"
#include <type_traits>
#include <utility>

namespace TK {

// Nullable and Unthread-Safe Reference Counting Pointer
// `T` must inherit `RefCounted<T>` publicly to use `RefPtr<T>`

template<typename T>
class [[nodiscard]] RefPtr {
public:
    ALWAYS_INLINE RefPtr() = default;
    ALWAYS_INLINE ~RefPtr() { clear(); }

    ALWAYS_INLINE RefPtr(decltype(nullptr)) : RefPtr() { }

    ALWAYS_INLINE explicit RefPtr(const T* ptr)
        : m_ptr(const_cast<T*>(ptr))
    {
        ref_if_not_null(m_ptr);
    }

    ALWAYS_INLINE RefPtr(const T& object)
        : m_ptr(const_cast<T*>(&object))
    {
        m_ptr->ref();
    }

    ALWAYS_INLINE RefPtr(const RefPtr& other)
        : m_ptr(other.m_ptr)
    {
        ref_if_not_null(m_ptr);
    }

    ALWAYS_INLINE RefPtr(const Ref<T>& other)
        : m_ptr(other.ptr())
    {
        m_ptr->ref();
    }

    // RefPtr(const OwnPtr<T>& other) = delete;

    ALWAYS_INLINE RefPtr(RefPtr&& other) noexcept
        : m_ptr(other.release())
    {
    }

    ALWAYS_INLINE RefPtr(Ref<T>&& other) noexcept
        : m_ptr(static_cast<T*>(other.release()))
    {
    }

    template<typename U> ALWAYS_INLINE RefPtr(const U* ptr) requires(std::is_base_of<T, U>::value)
        : m_ptr(const_cast<T*>(static_cast<const T*>(ptr)))
    {
        ref_if_not_null(m_ptr);
    }

    template<typename U> ALWAYS_INLINE RefPtr(const U& object) requires(std::is_base_of<T, U>::value)
        : m_ptr(const_cast<T*>(static_cast<const T*>(&object)))
    {
        m_ptr->ref();
    }

    template<typename U> ALWAYS_INLINE RefPtr(const RefPtr<U>& other) requires(std::is_base_of<T, U>::value)
        : m_ptr(const_cast<T*>(static_cast<const T*>(other.ptr())))
    {
        ref_if_not_null(m_ptr);
    }

    template<typename U> ALWAYS_INLINE RefPtr(const Ref<U>& other) requires(std::is_base_of<T, U>::value)
        : m_ptr(const_cast<T*>(static_cast<const T*>(other.ptr())))
    {
        m_ptr->ref();
    }

    template<typename U> ALWAYS_INLINE RefPtr(RefPtr<U>&& other) requires(std::is_base_of<T, U>::value)
        : m_ptr(static_cast<T*>(other.release()))
    {
    }

    template<typename U> ALWAYS_INLINE RefPtr(Ref<U>&& other) requires(std::is_base_of<T, U>::value)
        : m_ptr(const_cast<T*>(static_cast<const T*>(other.release())))
    {
    }

    ALWAYS_INLINE RefPtr& operator=(decltype(nullptr))
    {
        clear();
        return *this;
    }

    // ALWAYS_INLINE RefPtr& operator=(const T* ptr) { COPY_AND_SWAP(RefPtr, ptr); }
    // ALWAYS_INLINE RefPtr& operator=(const T& object) { COPY_AND_SWAP(RefPtr, object); }
    ALWAYS_INLINE RefPtr& operator=(const RefPtr& other)
    {
        RefPtr temp { other };
        swap(temp);
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(const Ref<T>& other)
    {
        RefPtr temp { other };
        swap(temp);
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(RefPtr&& other) noexcept
    {
        RefPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(Ref<T>&& other) noexcept
    {
        RefPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    // template<typename U> ALWAYS_INLINE RefPtr& operator=(const U* ptr) requires(std::is_base_of<T, U>::value) { COPY_AND_SWAP(RefPtr, ptr); }
    // template<typename U> ALWAYS_INLINE RefPtr& operator=(const U& object) requires(std::is_base_of<T, U>::value) { COPY_AND_SWAP(RefPtr, object); }
    template<typename U> ALWAYS_INLINE RefPtr& operator=(const RefPtr<U>& other) requires(std::is_base_of<T, U>::value)
    {
        RefPtr temp { other };
        swap(temp);
        return *this;
    }

    template<typename U> ALWAYS_INLINE RefPtr& operator=(const Ref<U>& other) requires(std::is_base_of<T, U>::value)
    {
        RefPtr temp { other };
        swap(temp);
        return *this;
    }

    template<typename U> ALWAYS_INLINE RefPtr& operator=(RefPtr<U>&& other) noexcept requires(std::is_base_of<T, U>::value)
    {
        RefPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    template<typename U> ALWAYS_INLINE RefPtr& operator=(Ref<U>&& other) noexcept requires(std::is_base_of<T, U>::value)
    {
        RefPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    ALWAYS_INLINE T* ptr() const { return as_ptr(); }
    ALWAYS_INLINE T* operator->() const { return as_nonnull_ptr(); }
    ALWAYS_INLINE T& operator*() const { return *as_nonnull_ptr(); }
    ALWAYS_INLINE operator T*() const { return as_ptr(); }
    ALWAYS_INLINE bool operator!() const { return is_null(); }
    ALWAYS_INLINE explicit operator bool() const { return !is_null(); }

    // Release the raw pointer without changing the reference count, also set the pointer it holds to `nullptr`
    // In some rare cases we need to transfer a `RefPtr` to a raw pointer
    [[nodiscard]] T* release()
    {
        return std::exchange(m_ptr, nullptr);
    }

    void swap(RefPtr& other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }

    template<typename U>
    void swap(RefPtr<U>& other) noexcept requires(std::is_base_of<T, U>::value)
    {
        std::swap(m_ptr, other.m_ptr);
    }

private:
    ALWAYS_INLINE bool is_null() const { return !m_ptr; }
    ALWAYS_INLINE T* as_ptr() const { return m_ptr; }
    ALWAYS_INLINE T* as_nonnull_ptr() const
    {
        ASSERT(m_ptr);
        return m_ptr;
    }

    ALWAYS_INLINE void ref_if_not_null(T* ptr)
    {
        if (!is_null()) [[likely]]
            ptr->ref();
    }

    ALWAYS_INLINE void deref_if_not_null(T* ptr)
    {
        if (!is_null()) [[likely]]
            ptr->deref();
    }

    ALWAYS_INLINE void clear()
    {
        deref_if_not_null(m_ptr);
        m_ptr = nullptr;
    }

private:
    T* m_ptr { nullptr };
};

template<typename T>
void swap(RefPtr<T>& a, RefPtr<T>& b)
{
    a.swap(b);
}

template<typename T, typename U>
void swap(RefPtr<T>& a, RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    a.swap(b);
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const RefPtr<T>& a, const RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() != b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const RefPtr<T>& a, const U* b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() == b;
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const RefPtr<T>& a, const U* b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() != b;
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const T* a, const RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const T* a, const RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a != b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const RefPtr<T>& a, Ref<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const RefPtr<T>& a, Ref<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() != b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator==(const Ref<T>& a, RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() == b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE bool operator!=(const Ref<T>& a, RefPtr<U>& b) requires(std::is_base_of<T, U>::value)
{
    return a.ptr() != b.ptr();
}

template<typename T>
ALWAYS_INLINE bool operator==(const RefPtr<T>& a, decltype(nullptr))
{
    return a.ptr() == nullptr;
}

template<typename T>
ALWAYS_INLINE bool operator!=(const RefPtr<T>& a, decltype(nullptr))
{
    return a.ptr() != nullptr;
}

template<typename T>
ALWAYS_INLINE bool operator==(decltype(nullptr), const RefPtr<T>& b)
{
    return nullptr == b.ptr();
}

template<typename T>
ALWAYS_INLINE bool operator!=(decltype(nullptr), const RefPtr<T>& b)
{
    return nullptr != b.ptr();
}

template<typename T, typename U>
ALWAYS_INLINE RefPtr<T> static_pointer_cast(const RefPtr<U>& p) requires(std::is_base_of<T, U>::value)
{
    return RefPtr<T>(static_cast<const T*>(p.ptr()));
}

template<typename T, typename U>
ALWAYS_INLINE RefPtr<T> const_pointer_cast(const RefPtr<U>& p) requires(std::is_base_of<T, U>::value)
{
    return RefPtr<T>(const_cast<T*>(p.ptr()));
}

template<typename T, typename U>
ALWAYS_INLINE RefPtr<T> dynamic_pointer_cast(const RefPtr<U>& p) requires(std::is_base_of<T, U>::value)
{
    return RefPtr<T>(dynamic_cast<T*>(p.ptr()));
}

template<typename T, typename U>
ALWAYS_INLINE RefPtr<T> reinterpret_pointer_cast(const RefPtr<U>& p) requires(std::is_base_of<T, U>::value)
{
    return RefPtr(reinterpret_cast<T*>(p.get()));
}

} // namespace TK
