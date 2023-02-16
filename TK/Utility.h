#pragma once

namespace TK {

template<typename T> struct is_lvalue_reference      { static constexpr bool value = false; };
template<typename T> struct is_lvalue_reference<T&>  { static constexpr bool value = true; };
template<typename T> struct is_rvalue_reference      { static constexpr bool value = false; };
template<typename T> struct is_rvalue_reference<T&&> { static constexpr bool value = true; };

template<typename T> struct remove_reference      { using type = T; };
template<typename T> struct remove_reference<T&>  { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };

template<typename T>
constexpr typename remove_reference<T>::type&& move(T&& arg) noexcept
{
  return static_cast<typename remove_reference<T>::type&&>(arg);
}

template<typename T>
void swap(T& a, T& b)
{
    T t = move(a);
    a = move(b);
    b = move(t);
}

// template<typename T>
// constexpr T&& forward(typename remove_reference<T>::type& arg) noexcept
// {
//     return static_cast<T&&>(arg);
// }

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type&& arg) noexcept
{
    static_assert(!is_lvalue_reference<T>::value, "cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(arg);
}

} // namespace TK
