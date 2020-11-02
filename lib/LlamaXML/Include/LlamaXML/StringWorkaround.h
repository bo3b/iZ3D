/*
 * Copyright (C) 2005 Llamagraphics, Inc.
 * 
 * This file is part of LlamaXML.
 *
 * LlamaXML is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LlamaXML is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 * LlamaXML is also available under an alternative commercial license for
 * developers who need the flexibility to include this library in proprietary
 * applications.  Visit http://www.llamagraphics.com/LlamaXML/ for more 
 * information.
 */

#ifndef LLAMAXML_STRINGWORKAROUND_H
#define LLAMAXML_STRINGWORKAROUND_H

#if (! __GNUC__) || __APPLE__
	#pragma once
#endif

#include "LlamaXML/LlamaStdInt.h"
#if __APPLE_CC__ && TARGET_OS_MAC && (! TARGET_OS_IPHONE)
#pragma GCC visibility push(default)
#endif
#include <string>
#if __APPLE_CC__ && TARGET_OS_MAC && (! TARGET_OS_IPHONE)
#pragma GCC visibility pop
#endif

#if defined(__GNUC__) && (__GNUC__ < 4)

namespace std {
	template<>
    struct char_traits<LlamaXML::uint16_t>
    {
      typedef LlamaXML::uint16_t	char_type;
      typedef unsigned long			int_type;
      typedef streampos				pos_type;
      typedef streamoff				off_type;
      typedef mbstate_t				state_type;

      static void
      assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static bool
      eq(const char_type& __c1, const char_type& __c2)
      { return __c1 == __c2; }

      static bool
      lt(const char_type& __c1, const char_type& __c2)
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
      for (size_t __i = 0; __i < __n; ++__i)
	if (lt(__s1[__i], __s2[__i]))
	  return -1;
	else if (lt(__s2[__i], __s1[__i]))
	  return 1;
      return 0;
    }

      static size_t
      length(const char_type* __s)
    {
      size_t __i = 0;
      while (!eq(__s[__i], char_type()))
        ++__i;
      return __i;
    }

      static const char_type*
      find(const char_type* __s, size_t __n, const char_type& __a)
    {
      for (size_t __i = 0; __i < __n; ++__i)
        if (eq(__s[__i], __a))
          return __s + __i;
      return 0;
    }

      static char_type*
      move(char_type* __s1, const char_type* __s2, size_t __n)
    {
      return static_cast<LlamaXML::uint16_t*>(std::memmove(__s1, __s2,
					       __n * sizeof(char_type)));
    }

      static char_type*
      copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
      std::copy(__s2, __s2 + __n, __s1);
      return __s1;
    }

      static char_type*
      assign(char_type* __s, size_t __n, char_type __a)
    {
      std::fill_n(__s, __n, __a);
      return __s;
    }

      static char_type
      to_char_type(const int_type& __c)
      { return static_cast<char_type>(__c); }

      static int_type
      to_int_type(const char_type& __c)
      { return static_cast<int_type>(__c); }

      static bool
      eq_int_type(const int_type& __c1, const int_type& __c2)
      { return __c1 == __c2; }

      static int_type
      eof()
      { return static_cast<int_type>(EOF); }

      static int_type
      not_eof(const int_type& __c)
      { return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type()); }
    };

#if 0
  template<>
    int
    char_traits<LlamaXML::uint16_t>::
    compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
      for (size_t __i = 0; __i < __n; ++__i)
	if (lt(__s1[__i], __s2[__i]))
	  return -1;
	else if (lt(__s2[__i], __s1[__i]))
	  return 1;
      return 0;
    }

  template<>
    size_t
    char_traits<LlamaXML::uint16_t>::
    length(const char_type* __p)
    {
      size_t __i = 0;
      while (!eq(__p[__i], char_type()))
        ++__i;
      return __i;
    }

  template<>
    const typename char_traits<LlamaXML::uint16_t>::char_type*
    char_traits<LlamaXML::uint16_t>::
    find(const char_type* __s, size_t __n, const char_type& __a)
    {
      for (size_t __i = 0; __i < __n; ++__i)
        if (eq(__s[__i], __a))
          return __s + __i;
      return 0;
    }

  template<>
    typename char_traits<LlamaXML::uint16_t>::char_type*
    char_traits<LlamaXML::uint16_t>::
    move(char_type* __s1, const char_type* __s2, size_t __n)
    {
      return static_cast<LlamaXML::uint16_t*>(std::memmove(__s1, __s2,
					       __n * sizeof(char_type)));
    }

  template<>
    typename char_traits<LlamaXML::uint16_t>::char_type*
    char_traits<LlamaXML::uint16_t>::
    copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
      std::copy(__s2, __s2 + __n, __s1);
      return __s1;
    }

  template<>
    typename char_traits<LlamaXML::uint16_t>::char_type*
    char_traits<LlamaXML::uint16_t>::
    assign(char_type* __s, size_t __n, char_type __a)
    {
      std::fill_n(__s, __n, __a);
      return __s;
    }
#endif
}
#elif defined(__GNUC__) && ! defined(_GLIBCXX_USE_WCHAR_T)
namespace std {
    template<> struct char_traits<wchar_t>;

    typedef basic_string<wchar_t> wstring;
}
#endif

#endif
