#pragma once

#include <type_traits>

namespace TK {

template<typename Callback>
class ScopeGuard {
public:
    ScopeGuard(Callback callback)
        : m_callback(std::move(callback))
    {
    }

    ~ScopeGuard()
    {
        m_callback();
    }

private:
    Callback m_callback;
};

} // namespace TK
