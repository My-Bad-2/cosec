#pragma once

#include <bits/iterator_base.h>
#include <cstddef>
#include <cstdint>

namespace std {
template <typename _InputIterator>
inline constexpr typename iterator_traits<_InputIterator>::difference_type
__distance(_InputIterator __first, _InputIterator __last, input_iterator_tag) {

    typename iterator_traits<_InputIterator>::difference_type __n = 0;

    while (__first != __last) {
        ++__first;
        ++__n;
    }

    return __n;
}

template <typename _RandomAccessIterator>
inline constexpr
    typename iterator_traits<_RandomAccessIterator>::difference_type
    __distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
               random_access_iterator_tag) {
    return __last - __first;
}

template <typename _OutputIterator>
void __distance(_OutputIterator, _OutputIterator, output_iterator_tag) = delete;

template <typename _InputIterator>
[[nodiscard]] inline constexpr
    typename iterator_traits<_InputIterator>::difference_type
    distance(_InputIterator __first, _InputIterator __last) {

    return std::__distance(__first, __last, std::__iterator_category(__first));
}

template <typename _InputIterator, typename _Distance>
inline constexpr void __advance(_InputIterator& __i, _Distance __n,
                                input_iterator_tag) {
    assert(__n >= 0);
    while (__n--) {
        ++__i;
    }
}

template <typename _BidirectionalIterator, typename _Distance>
inline constexpr void __advance(_BidirectionalIterator& __i, _Distance __n,
                                bidirectional_iterator_tag) {
    if (__n > 0) {
        while (__n--) {
            ++__i;
        }
    } else {
        while (__n++) {
            --__i;
        }
    }
}

template <typename _RandomAccessIterator, typename _Distance>
inline constexpr void __advance(_RandomAccessIterator& __i, _Distance __n,
                                random_access_iterator_tag) {
    if (__builtin_constant_p(__n) && __n == 1) {
        ++__i;
    } else if (__builtin_constant_p(__n) && __n == -1) {
        --__i;
    } else {
        __i += __n;
    }
}

template <typename _InputIterator, typename _Distance>
inline constexpr void advance(_InputIterator& __i, _Distance __n) {
    // concept requirements -- taken care of in __advance
    typename iterator_traits<_InputIterator>::difference_type __d = __n;
    std::__advance(__i, __d, std::__iterator_category(__i));
}

template <typename _InputIterator>
[[nodiscard]] inline constexpr _InputIterator next(
    _InputIterator __x,
    typename iterator_traits<_InputIterator>::difference_type __n = 1) {
    std::advance(__x, __n);
    return __x;
}

template <typename _BidirectionalIterator>
[[nodiscard]] inline constexpr _BidirectionalIterator prev(
    _BidirectionalIterator __x,
    typename iterator_traits<_BidirectionalIterator>::difference_type __n = 1) {
    std::advance(__x, -__n);
    return __x;
}

}  // namespace std