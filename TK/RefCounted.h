#pragma once

#include "Assertions.h"
#include "Definitions.h"
#include "NonCopyable.h"
#include "NonMovable.h"

namespace TK {

namespace Internal {

// The `RefCounted` class inherits from it reducing the template bloat
class RefCountedBase {
    TK_MAKE_NONCOPYABLE(RefCountedBase)
    TK_MAKE_NONMOVABLE(RefCountedBase)

public:
    ALWAYS_INLINE void ref() const { m_ref_cnt++; }
    ALWAYS_INLINE unsigned ref_count() const { return m_ref_cnt; }

protected:
    RefCountedBase() = default;
    ~RefCountedBase() = default;

    // Returns whether the pointer should be freed or not
    ALWAYS_INLINE bool deref_base() const
    {
        ASSERT(m_ref_cnt > 0);
        m_ref_cnt--;
        if (m_ref_cnt == 0)
            return true;
        return false;
    }

private:
    mutable unsigned m_ref_cnt { 0 };
};

} // namespace Internal

template<typename T>
class RefCounted : public Internal::RefCountedBase {
    TK_MAKE_NONCOPYABLE(RefCounted)

public:
    ALWAYS_INLINE void deref() const
    {
        if (deref_base())
            delete const_cast<T*>(static_cast<const T*>(this));
    }

protected:
    RefCounted()
    {
    }

    ~RefCounted()
    {
    }
};

} // namespace TK
