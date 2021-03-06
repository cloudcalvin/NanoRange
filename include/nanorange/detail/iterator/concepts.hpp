// nanorange/detail/iterator/concepts.hpp
//
// Copyright (c) 2018 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NANORANGE_DETAIL_ITERATOR_CONCEPTS_HPP_INCLUDED
#define NANORANGE_DETAIL_ITERATOR_CONCEPTS_HPP_INCLUDED

#include <nanorange/detail/concepts/object.hpp>
#include <nanorange/detail/iterator/associated_types.hpp>
#include <nanorange/detail/iterator/traits.hpp>

NANO_BEGIN_NAMESPACE

// [range.iterators.readable]
namespace detail {

struct Readable_req {
    template <typename In>
    auto requires_()
        -> decltype(std::declval<iter_value_t<In>>(),
                               std::declval<iter_reference_t<In>>(),
                               std::declval<iter_rvalue_reference_t<In>>());
};

template <typename>
auto Readable_fn(long) -> std::false_type;

template <typename In>
auto Readable_fn(int) -> std::enable_if_t<
     requires_<Readable_req, In> &&
     CommonReference<iter_reference_t<In>&&, iter_value_t<In>&> &&
     CommonReference<iter_reference_t<In>&&, iter_rvalue_reference_t<In>&&> &&
     CommonReference<iter_rvalue_reference_t<In>&&, const iter_value_t<In>&>,
             std::true_type>;

} // namespace detail

template <typename In>
NANO_CONCEPT Readable = decltype(detail::Readable_fn<In>(0))::value;

// [range.iterators.writable]
namespace detail {

struct Writable_req {
    template <typename Out, typename T>
    auto requires_(Out&& o, T&& t) -> decltype(valid_expr(
        *o = std::forward<T>(t), *std::forward<Out>(o) = std::forward<T>(t),
        const_cast<const iter_reference_t<Out>&&>(*o) = std::forward<T>(t),
        const_cast<const iter_reference_t<Out>&&>(*std::forward<Out>(o)) =
            std::forward<T>(t)));
};

} // namespace detail

template <typename Out, typename T>
NANO_CONCEPT Writable = detail::requires_<detail::Writable_req, Out, T>;

// [range.iterators.weaklyincrementable]

namespace detail {

template <typename T, typename Deduced>
auto same_lv(Deduced&) -> std::enable_if_t<Same<T, Deduced>, int>;

struct WeaklyIncrementable_req {
    template <typename I>
    auto requires_(I i) -> decltype(
        std::declval<iter_difference_t<I>>(),
        requires_expr<SignedIntegral<iter_difference_t<I>>>{},
        same_lv<I>(++i), i++);
};

} // namespace detail

template <typename I>
NANO_CONCEPT WeaklyIncrementable =
    Semiregular<I>&& detail::requires_<detail::WeaklyIncrementable_req, I>;

// [range.iterators.incrementable]

namespace detail {

struct Incrementable_req {
    template <typename I>
    auto requires_(I i) -> decltype(requires_expr<Same<decltype(i++), I>>{});
};

} // namespace detail

template <typename I>
NANO_CONCEPT Incrementable = Regular<I>&& WeaklyIncrementable<I>&&
    detail::requires_<detail::Incrementable_req, I>;

// [range.iterators.iterator]

namespace detail {

struct Iterator_req {
    template <typename I>
    auto requires_(I i) -> decltype(*i,
            requires_expr<CanReference<decltype(*i)>>{});
};

} // namespace detail

template <typename I>
NANO_CONCEPT Iterator =
    detail::requires_<detail::Iterator_req, I> && WeaklyIncrementable<I>;

// [range.iterators.sentinel]

template <typename S, typename I>
NANO_CONCEPT Sentinel =
    Semiregular<S>&& Iterator<I>&& detail::WeaklyEqualityComparableWith<S, I>;

// [range.iterators.sizedsentinel]

template <typename S, typename I>
constexpr bool disable_sized_sentinel = false;

namespace detail {

struct SizedSentinel_req {
    template <typename S, typename I>
    auto requires_(const S& s, const I& i)
        -> decltype(requires_expr<Same<decltype(s - i), iter_difference_t<I>>>{},
                    requires_expr<Same<decltype(i - s), iter_difference_t<I>>>{});
};

} // namespace detail

template <typename S, typename I>
NANO_CONCEPT SizedSentinel =
    Sentinel<S, I> &&
    !disable_sized_sentinel<std::remove_cv_t<S>, std::remove_cv_t<I>> &&
    detail::requires_<detail::SizedSentinel_req, S, I>;

// This is a hack, but I'm fed up with my tests breaking because GCC
// has a silly extension
template <typename S>
NANO_CONCEPT SizedSentinel<S, void*> = false;

template <typename I>
NANO_CONCEPT SizedSentinel<void*, I> = false;

template <>
NANO_CONCEPT SizedSentinel<void*, void*> = false;

// [range.iterators.input]

namespace detail {

template <typename>
auto InputIterator_fn(long) -> std::false_type;

template <typename I>
auto InputIterator_fn(int) -> std::enable_if_t<
    Iterator<I> && Readable<I> &&
    exists_v<iterator_category_t, I> &&
    DerivedFrom<iterator_category_t<I>, input_iterator_tag>,
            std::true_type>;


}

template <typename I>
NANO_CONCEPT InputIterator = decltype(detail::InputIterator_fn<I>(0))::value;

// [ranges.iterator.output]

namespace detail {

struct OutputIterator_req {
    template <typename I, typename T>
    auto requires_(I i, T&& t)
        -> decltype(valid_expr(*i++ = std::forward<T>(t)));
};

} // namespace detail

template <typename I, typename T>
NANO_CONCEPT OutputIterator = Iterator<I>&& Writable<I, T>&&
    detail::requires_<detail::OutputIterator_req, I, T>;

// [ranges.iterators.forward]

namespace detail {

template <typename>
auto ForwardIterator_fn(long) -> std::false_type;

template <typename I>
auto ForwardIterator_fn(int) -> std::enable_if_t<
        InputIterator<I> &&
        DerivedFrom<iterator_category_t<I>, forward_iterator_tag> &&
        Incrementable<I> &&
        Sentinel<I, I>,
                std::true_type>;

}

template <typename I>
NANO_CONCEPT ForwardIterator = decltype(detail::ForwardIterator_fn<I>(0))::value;

// [ranges.iterators.bidirectional]

namespace detail {

struct BidirectionalIterator_req {
    template <typename I>
    auto requires_(I i)
        -> decltype(same_lv<I>(--i), requires_expr<Same<decltype(i--), I>>{});
};

template <typename>
auto BidirectionalIterator_fn(long) -> std::false_type;

template <typename I>
auto BidirectionalIterator_fn(int) -> std::enable_if_t<
        ForwardIterator<I> &&
        DerivedFrom<iterator_category_t<I>, bidirectional_iterator_tag> &&
        requires_<BidirectionalIterator_req, I>,
                std::true_type>;

} // namespace detail

template <typename I>
NANO_CONCEPT BidirectionalIterator =
    decltype(detail::BidirectionalIterator_fn<I>(0))::value;

// [ranges.iterators.random.access]

namespace detail {

struct RandomAccessIterator_req {
    template <typename I>
    auto requires_(I i, const I j, const iter_difference_t<I> n) -> decltype(
        valid_expr(same_lv<I>(i += n),
                   j + n, requires_expr<Same<decltype(j + n), I>>{},
                   n + j,
#ifndef _MSC_VER
                   requires_expr<Same<decltype(n + j), I>>{}, // FIXME: MSVC doesn't like this when I = int*
#endif
                   same_lv<I>(i -= n),
                   j - n, requires_expr<Same<decltype(j - n), I>>{},
                   j[n],
                   requires_expr<Same<decltype(j[n]), iter_reference_t<I>>>{}));
};

template <typename>
auto RandomAccessIterator_fn(long) -> std::false_type;

template <typename I>
auto RandomAccessIterator_fn(int) -> std::enable_if_t<
     BidirectionalIterator<I> &&
     DerivedFrom<iterator_category_t<I>, random_access_iterator_tag> &&
     StrictTotallyOrdered<I> &&
     SizedSentinel<I, I> &&
     requires_<RandomAccessIterator_req, I>,
             std::true_type>;

} // namespace detail

template <typename I>
NANO_CONCEPT RandomAccessIterator = 
        decltype(detail::RandomAccessIterator_fn<I>(0))::value;

namespace detail {

template <typename>
auto ContiguousIterator_fn(long) -> std::false_type;

template <typename I>
auto ContiguousIterator_fn(int) -> std::enable_if_t<
    RandomAccessIterator<I> &&
    DerivedFrom<iterator_category_t<I>, contiguous_iterator_tag> &&
    std::is_lvalue_reference<iter_reference_t<I>>::value &&
    Same<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>>,
            std::true_type>;

}

template <typename I>
NANO_CONCEPT ContiguousIterator = decltype(detail::ContiguousIterator_fn<I>(0))::value;

NANO_END_NAMESPACE

#endif
