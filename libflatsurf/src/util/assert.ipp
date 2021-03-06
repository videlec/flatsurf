/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2019 Julian Rüth
 *
 *  Flatsurf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Flatsurf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with flatsurf. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#ifndef LIBFLATSURF_UTIL_ASSERT_IPP
#define LIBFLATSURF_UTIL_ASSERT_IPP

#include <boost/preprocessor/stringize.hpp>

#include "false.ipp"

// Run a (cheap) check that a (user provided) argument is valid.
// If the check should be disabled when NDEBUG is defined, e.g., because it
// occurs in a hotspot, use ASSERT_ARGUMENT instead.
#define CHECK_ARGUMENT_(CONDITION)                                             \
  while (not(CONDITION)) {                                                     \
    throw std::invalid_argument(#CONDITION                                     \
                                " in " __FILE__ BOOST_PP_STRINGIZE(__LINE__)); \
  }
#define CHECK_ARGUMENT(CONDITION, MESSAGE)                                    \
  while (not(CONDITION)) {                                                    \
    throw std::invalid_argument(                                              \
        #CONDITION " " MESSAGE " in " __FILE__ BOOST_PP_STRINGIZE(__LINE__)); \
  }

#ifdef NDEBUG
#define ASSERT_ARGUMENT_(CONDITION) \
  while (false) {                   \
  }
#define ASSERT_ARGUMENT(CONDITION, MESSAGE) \
  while (false) {                           \
  }
#else
#define ASSERT_ARGUMENT_(CONDITION) CHECK_ARGUMENT_(CONDITION)
#define ASSERT_ARGUMENT(CONDITION, MESSAGE) CHECK_ARGUMENT(CONDITION, MESSAGE)
#endif

template <typename _ = void>
inline void assert_unreachable() {
  static_assert(flatsurf::false_value_v<_>, "This code path can not get instantiated by the compiler…or at least that's what we thought.");
}

#endif
