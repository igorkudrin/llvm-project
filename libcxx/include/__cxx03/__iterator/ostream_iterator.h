// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CXX03___ITERATOR_OSTREAM_ITERATOR_H
#define _LIBCPP___CXX03___ITERATOR_OSTREAM_ITERATOR_H

#include <__cxx03/__config>
#include <__cxx03/__fwd/ostream.h>
#include <__cxx03/__fwd/string.h>
#include <__cxx03/__iterator/iterator.h>
#include <__cxx03/__iterator/iterator_traits.h>
#include <__cxx03/__memory/addressof.h>
#include <__cxx03/cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

_LIBCPP_SUPPRESS_DEPRECATED_PUSH
template <class _Tp, class _CharT = char, class _Traits = char_traits<_CharT> >
class _LIBCPP_TEMPLATE_VIS ostream_iterator : public iterator<output_iterator_tag, void, void, void, void> {
  _LIBCPP_SUPPRESS_DEPRECATED_POP

public:
  typedef output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef basic_ostream<_CharT, _Traits> ostream_type;

private:
  ostream_type* __out_stream_;
  const char_type* __delim_;

public:
  _LIBCPP_HIDE_FROM_ABI ostream_iterator(ostream_type& __s) _NOEXCEPT
      : __out_stream_(std::addressof(__s)),
        __delim_(nullptr) {}
  _LIBCPP_HIDE_FROM_ABI ostream_iterator(ostream_type& __s, const _CharT* __delimiter) _NOEXCEPT
      : __out_stream_(std::addressof(__s)),
        __delim_(__delimiter) {}
  _LIBCPP_HIDE_FROM_ABI ostream_iterator& operator=(const _Tp& __value) {
    *__out_stream_ << __value;
    if (__delim_)
      *__out_stream_ << __delim_;
    return *this;
  }

  _LIBCPP_HIDE_FROM_ABI ostream_iterator& operator*() { return *this; }
  _LIBCPP_HIDE_FROM_ABI ostream_iterator& operator++() { return *this; }
  _LIBCPP_HIDE_FROM_ABI ostream_iterator& operator++(int) { return *this; }
};

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___CXX03___ITERATOR_OSTREAM_ITERATOR_H
