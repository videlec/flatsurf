/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2018 Alex Eskin
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

// This is an assorted list of defines that should be controlled by
// autoconf. The actual defines should probably not be included by the other
// header files or at least be namespaced with a LIBFLATSURF prefix.

#ifndef FLATSURF_DEFS_H
#define FLATSURF_DEFS_H

#include <gmpxx.h>
#include <complex>
#include <string>

// RETURN HERE
#ifdef i686
//#define USE_LONG_DOUBLE
//#define USE_QUAD
#endif

#ifdef x86_64
//#define USE_LONG_DOUBLE
//#define USE_QUAD
#endif

#ifdef USE_QUAD
#include <boost/multiprecision/float128.hpp>
using namespace boost::multiprecision;
#define COORD float128
#elif defined USE_LONG_DOUBLE
#define COORD long double
#else
#define COORD double
#endif

// change later
typedef std::complex<COORD> Point;

#ifdef USE_MPI
#define USE_PARALLEL
#endif

#ifdef USE_PVM
#define USE_PARALLEL
#endif

#ifdef USE_MPI
#include <iostream.h>
#include <mpi++.h>
#endif

#ifdef USE_PVM
extern "C" {
#include <pvm3.h>
}
#endif

__attribute__((noreturn)) void ERR_RET(const std::string &s);
__attribute__((noreturn)) void ERR_RET2(const std::string &s, int t);

typedef mpq_class bigrat;

#define CONG_SAME 0
#define CONG_REVERSE 1
#define CONG_FALSE 2

#define MAX_VERTEX_DEGREE 50
#define MAX_SADDLES 102
#define MAX_VERTICES 10
#define MAX_SADDLECONFS 100

#define MAX_HOSTNAME 50

#define UNDEFINED -1000

#define NOT_HIT 0
#define HIT_RELEVANT 1

#endif  // FLATSURF_DEFS_H
