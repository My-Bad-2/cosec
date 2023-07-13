#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace std {
template <typename _Base, typename _Derived>
struct is_base_of
    : public integral_constant<bool, __is_base_of(_Base, _Derived)> {};

struct input_iterator_tag {};

struct output_iterator_tag {};

struct forward_iterator_tag : public input_iterator_tag {};

struct bidirectional_iterator_tag : public forward_iterator_tag {};

struct random_access_iterator_tag : public bidirectional_iterator_tag {};

struct contiguous_iterator_tag : public random_access_iterator_tag {};

template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
           typename _Pointer = _Tp*, typename _Reference = _Tp&>
    struct [[__deprecated__]] iterator {
      /// One of the @link iterator_tags tag types@endlink.
      typedef _Category  iterator_category;
      /// The type "pointed to" by the iterator.
      typedef _Tp        value_type;
      /// Distance between iterators is represented as this type.
      typedef _Distance  difference_type;
      /// This type represents a pointer-to-value_type.
      typedef _Pointer   pointer;
      /// This type represents a reference-to-value_type.
      typedef _Reference reference;
    };

template <typename _Iterator>
struct iterator_traits;

template <typename _Iterator, typename = void_t<>>
struct __iterator_traits {};

template <typename _Iterator>
struct __iterator_traits<
    _Iterator,
    void_t<typename _Iterator::iterator_category,
           typename _Iterator::value_type, typename _Iterator::difference_type,
           typename _Iterator::pointer, typename _Iterator::reference>> {
    typedef typename _Iterator::iterator_category iterator_category;
    typedef typename _Iterator::value_type value_type;
    typedef typename _Iterator::difference_type difference_type;
    typedef typename _Iterator::pointer pointer;
    typedef typename _Iterator::reference reference;
};

template <typename _Tp>
struct iterator_traits<_Tp*> {
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef ptrdiff_t difference_type;
    typedef _Tp* pointer;
    typedef _Tp& reference;
};

template <typename _Tp>
struct iterator_traits<const _Tp*> {
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef ptrdiff_t difference_type;
    typedef const _Tp* pointer;
    typedef const _Tp& reference;
};

template <typename _Iter>
inline constexpr typename iterator_traits<_Iter>::iterator_category
__iterator_category(const _Iter&) {
    return typename iterator_traits<_Iter>::iterator_category();
}

template <typename _Iter>
using __iterator_category_t =
    typename iterator_traits<_Iter>::iterator_category;

template <typename _InIter>
using _RequireInputIter = enable_if_t<
    is_convertible<__iterator_category_t<_InIter>, input_iterator_tag>::value>;

template <typename _It, typename _Cat = __iterator_category_t<_It>>
struct __is_random_access_iter : is_base_of<random_access_iterator_tag, _Cat> {
    typedef is_base_of<random_access_iterator_tag, _Cat> _Base;

    enum { __value = _Base::value };
};
}  // namespace std