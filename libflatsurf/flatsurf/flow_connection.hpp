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

#ifndef LIBFLATSURF_FLOW_CONNECTION_HPP
#define LIBFLATSURF_FLOW_CONNECTION_HPP

#include <iosfwd>
#include <boost/operators.hpp>

#include "copyable.hpp"

namespace flatsurf {
template <typename Surface>
class FlowConnection :
  Serializable<FlowConnection<Surface>>,
  boost::equality_comparable<FlowConnection<Surface>> {
  // Flow connections cannot be created directly (other than copying & moving them.)
  // They are created as products of FlowDecomposition.
  FlowConnection();

  static_assert(std::is_same_v<Surface, std::decay_t<Surface>>, "type must not have modifiers such as const");

 public:
  FlowComponent<Surface> component() const;

  FlowConnection<Surface> operator-() const;

  // TODO: Globally rename circumference to perimeter.

  // If this is a vertical connection, return the MaybeVerticalFlowConnection
  // from which this connection was created.
  MaybeVerticalFlowConnection<Surface> source() const;

  // If this is not a vertical connection, return the
  // MaybeVerticalFlowConnection that is counterclockwise next to this
  // connection.
  MaybeVerticalFlowConnection<Surface> nextAtSingularity() const;

  // If this is not a vertical connection, return the
  // MaybeVerticalFlowConnection that is clockwise next to this
  // connection.
  MaybeVerticalFlowConnection<Surface> previousAtSingularity();

  FlowConnection<Surface> nextInCircumference() const noexcept;

  FlowConnection<Surface> previousInCircumference() const noexcept;

  // Return whether this is a vertical (parallel or antiparallel) connection
  // with respect to the fixed vertical direction.
  bool vertical() const;

  bool parallel() const;

  bool antiparallel() const;

  // Return whether this edge is a (vertical) boundary between two distinct FlowComponents.
  bool boundary() const;

  bool operator==(const FlowConnection<Surface>&) const noexcept;

  typename Surface::SaddleConnection saddleConnection() const noexcept;

  template <typename S>
  friend std::ostream& operator<<(std::ostream&, const FlowConnection<S>&);

 private:
  using Implementation = ::flatsurf::Implementation<FlowConnection>;
  Copyable<Implementation> impl;
  friend Implementation;
};
}

namespace std {

template <typename Surface>
struct hash<::flatsurf::FlowConnection<Surface>> { std::size_t operator()(const ::flatsurf::FlowConnection<Surface>&) const noexcept; };

}

#endif
