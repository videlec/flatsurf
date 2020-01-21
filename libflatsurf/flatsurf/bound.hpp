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

#ifndef LIBFLATSURF_BOUND_HPP
#define LIBFLATSURF_BOUND_HPP

#include <boost/operators.hpp>

#include "forward.hpp"

namespace flatsurf {

class Bound : boost::equality_comparable<Bound> {
 public:
  Bound(long x, long y);

  long long squared() const noexcept;

  bool operator==(const Bound&) const noexcept;

 private:
  long long square;
};

}

#endif