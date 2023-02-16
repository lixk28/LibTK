#pragma once

#include "RefCounted.h"
#include "RefPtr.h"
#include "NonCopyable.h"
#include <utility>
#include <type_traits>

namespace TK {

template<typename T>
class Weakable;
template<typename T>
class WeakPtr;

namespace Internal {

class WeakFlag : public RefCounted<WeakFlag> {
public:
    bool is_valid() const { return m_ptr != nullptr; }
    void invalidate() const { m_ptr = nullptr; }

    template<typename T>
    T* unsafe_ptr() const { return static_cast<T*>(m_ptr); }

private:
    template<typename T>
    friend class TK::Weakable;

    template<typename T>
    friend class TK::WeakPtr;

    template<typename T>
    explicit WeakFlag(T* ptr)
        : m_ptr(ptr)
    {
    }

    mutable void* m_ptr { nullptr };
};

} // namespace Internal

template<typename T>
class Weakable {
    TK_MAKE_NONCOPYABLE(Weakable)

public:
    WeakPtr<T> weak_from_this() const
    {
        if (!m_flag)
            m_flag = RefPtr<Internal::WeakFlag> { new Internal::WeakFlag(const_cast<T*>(static_cast<const T*>(this))) };

        return WeakPtr<T> { m_flag };
    }

protected:
    Weakable() = default;

    ~Weakable()
    {
        if (m_flag)
            m_flag->invalidate();
    }

private:
    mutable RefPtr<Internal::WeakFlag> m_flag { nullptr };
};

} // namespace TK
