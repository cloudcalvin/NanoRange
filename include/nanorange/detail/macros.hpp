// nanorange/detail/macros.hpp
//
// Copyright (c) 2018 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NANORANGE_DETAIL_MACROS_HPP_INCLUDED
#define NANORANGE_DETAIL_MACROS_HPP_INCLUDED

#include <ciso646>

#if (__cplusplus >= 201703) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define NANO_HAVE_CPP17
#endif

#if defined(NANO_HAVE_CPP17) || defined(__cpp_inline_variables)
#define NANO_HAVE_INLINE_VARS
#endif

#ifdef NANORANGE_NO_DEPRECATION_WARNINGS
#define NANO_DEPRECATED
#define NANO_DEPRECATED_FOR(x)
#else
#define NANO_DEPRECATED [[deprecated]]
#define NANO_DEPRECATED_FOR(x) [[deprecated(x)]]
#endif

#ifdef NANO_HAVE_CPP17
#define NANO_NODISCARD [[nodiscard]]
#else
#define NANO_NODISCARD
#endif

#if defined(NANO_HAVE_CPP17) || defined(__cpp_deduction_guides)
#define NANO_HAVE_DEDUCTION_GUIDES
#endif

#define NANO_CONCEPT constexpr bool

#define NANO_BEGIN_NAMESPACE                                                   \
    \
namespace nano                                                                 \
    {                                                                          \
        \
inline namespace ranges                                                        \
        {

#define NANO_END_NAMESPACE                                                     \
    }                                                                          \
    }

#ifdef NANO_HAVE_INLINE_VARS
#define NANO_INLINE_VAR(type, name)                                            \
    inline namespace function_objects {                                        \
    inline constexpr type name{};                                              \
    }
#define NANO_INLINE_VARIABLE inline
#else
#define NANO_INLINE_VAR(type, name)                                            \
    inline namespace function_objects {                                        \
    inline namespace {                                                         \
    constexpr const auto& name =                                               \
        ::nano::ranges::detail::static_const_<type>::value;                    \
    }                                                                          \
    }
#define NANO_INLINE_VARIABLE
#endif

NANO_BEGIN_NAMESPACE

namespace detail {

template <typename T>
struct static_const_ {
    static constexpr T value{};
};

template <typename T>
constexpr T static_const_<T>::value;

} // namespace detail

NANO_END_NAMESPACE

#if defined(_LIBCPP_VERSION)
#define NANO_BEGIN_NAMESPACE_STD _LIBCPP_BEGIN_NAMESPACE_STD
#define NANO_END_NAMESPACE_STD _LIBCPP_END_NAMESPACE_STD
#elif defined(_MSVC_STL_VERSION)
#define NANO_BEGIN_NAMESPACE_STD _STD_BEGIN
#define NANO_END_NAMESPACE_STD _STD_END
#elif defined(_GLIBCXX_DEBUG)
#ifndef NANORANGE_NO_STD_FORWARD_DECLARATIONS
#define NANORANGE_NO_STD_FORWARD_DECLARATIONS
#endif
#else
#define NANO_BEGIN_NAMESPACE_STD namespace std {
#define NANO_END_NAMESPACE_STD }
#endif


#endif
