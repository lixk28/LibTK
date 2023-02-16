#pragma once

#include "Assertions.h"
#include "NonCopyable.h"
#include "NonMovable.h"
#include "OwnPtr.h"
#include <utility>

namespace TK {

template<typename T>
class Function;

template<typename ReturnType, typename... ArgTypes>
class Function<ReturnType (ArgTypes...)> {
    TK_MAKE_NONCOPYABLE(Function)

private:
    class CallableWrapperBase {
    public:
        virtual ~CallableWrapperBase() = default;
        virtual ReturnType invoke(ArgTypes... args) = 0;
    };

    template<typename CallableType>
    class CallableWrapper final : public CallableWrapperBase {
        TK_MAKE_NONCOPYABLE(CallableWrapper)
        TK_MAKE_NONMOVABLE(CallableWrapper)

    public:
        explicit CallableWrapper(CallableType&& callable)
            : m_callable(std::move(callable))
        {
        }

        ~CallableWrapper() = default;

        ReturnType invoke(ArgTypes... args) override { return m_callable(std::forward<ArgTypes>(args)...); }

    private:
        CallableType m_callable;
    };

public:
    Function() = default;
    ~Function() = default;

    Function(decltype(nullptr))
        : Function()
    {
    }

    template<typename CallableType>
    Function(CallableType&& callable)
        : m_callable_wrapper(make_own<CallableWrapper<CallableType>>(std::forward<CallableType>(callable)))
    {
    }

    template<typename FunctionType>
    Function(FunctionType func)
        : m_callable_wrapper(make_own<CallableWrapper<FunctionType>>(std::forward<FunctionType>(func)))
    {
    }

    // Function(Function&& other)
    // {
    // }

    template<typename CallableType>
    Function& opeartor=(CallableType&& callable)
    {
        m_callable_wrapper = make_own<CallableWrapper<CallableType>>(std::forward<CallableType>(callable));
        return *this;
    }

    template<typename FunctionType>
    Function& operator=(FunctionType func)
    {
        m_callable_wrapper = make_own<CallableWrapper<FunctionType>>(std::forward<FunctionType>(func));
        return *this;
    }

    Function& operator=(decltype(nullptr))
    {
        m_callable_wrapper = nullptr;
        return *this;
    }

    // Function& operator=(Function&& other)
    // {
    // }

    ReturnType operator()(ArgTypes... args) const
    {
        ASSERT(m_callable_wrapper);
        return m_callable_wrapper->invoke(std::forward<ArgTypes>(args)...);
    }

    explicit operator bool() const { return !!m_callable_wrapper; }

private:
    OwnPtr<CallableWrapperBase> m_callable_wrapper { nullptr };
};

}
