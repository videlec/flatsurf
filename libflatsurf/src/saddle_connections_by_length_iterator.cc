/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2020 Julian Rüth
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

#include <iterator>

#include "../flatsurf/saddle_connections_iterator.hpp"
#include "../flatsurf/vector.hpp"
#include "external/rx-ranges/include/rx/ranges.hpp"
#include "impl/saddle_connections_by_length.impl.hpp"
#include "impl/saddle_connections_by_length_iterator.impl.hpp"
#include "util/assert.ipp"

namespace flatsurf {

template <typename Surface>
void SaddleConnectionsByLengthIterator<Surface>::increment() {
  ASSERT(!impl->connectionsWithinBounds.empty(), "cannot increment iterator at end");
  impl->connectionsWithinBounds.pop_front();
  if (impl->connectionsWithinBounds.empty())
    impl->increment();
}

template <typename Surface>
const SaddleConnection<Surface>& SaddleConnectionsByLengthIterator<Surface>::dereference() const {
  ASSERT(!impl->connectionsWithinBounds.empty(), "Cannot dereference iterator at end.");
  return impl->connectionsWithinBounds.front();
}

template <typename Surface>
bool SaddleConnectionsByLengthIterator<Surface>::equal(const SaddleConnectionsByLengthIterator& rhs) const {
  return impl->upperBoundInclusive == rhs.impl->upperBoundInclusive && &impl->connections == &rhs.impl->connections && impl->lowerBoundExclusive == rhs.impl->lowerBoundExclusive && impl->connectionsWithinBounds.size() == rhs.impl->connectionsWithinBounds.size();
}

template <typename Surface>
std::ostream& operator<<(std::ostream& os, const SaddleConnectionsByLengthIterator<Surface>&) {
  return os << "SaddleConnectionsByLengthIterator()";
}

template <typename Surface>
ImplementationOf<SaddleConnectionsByLengthIterator<Surface>>::ImplementationOf(const SaddleConnectionsByLength<Surface>& connections) :
  connections(connections),
  lowerBoundExclusive(0),
  upperBoundInclusive(0),
  connectionsWithinBounds() {}

template <typename Surface>
void ImplementationOf<SaddleConnectionsByLengthIterator<Surface>>::increment() {
  ASSERT(connectionsWithinBounds.empty(), "cannot increment if not all connections have been consumed yet");
  while (connectionsWithinBounds.empty()) {
    if (upperBoundInclusive == 0) {
      // Start with only the shortest connections.
      lowerBoundExclusive = 0;
      upperBoundInclusive = Bound::upper(connections.surface().shortest());
    } else {
      // Then double the bound we are considering each time.
      lowerBoundExclusive = upperBoundInclusive;
      upperBoundInclusive = lowerBoundExclusive * 2;
    }
    if (connections.bound() && lowerBoundExclusive >= *connections.bound()) {
      // We are at the end of the search. Set this to be identical to the end() iterator.
      lowerBoundExclusive = 0;
      upperBoundInclusive = 0;
      break;
    }

    // Fill connectionsWithinBounds with all connections between [lowerBoundInclusive, upperBoundExclusive)
    auto withinBounds = connections.byAngle().bound(upperBoundInclusive) | rx::filter([&](const auto& connection) { return connection > lowerBoundExclusive; }) | rx::to_vector();
    std::sort(begin(withinBounds), end(withinBounds), [](const auto& lhs, const auto& rhs) {
      const auto a = lhs.vector();
      const auto b = rhs.vector();
      return a * a < b * b;
    });
    std::copy(rbegin(withinBounds), rend(withinBounds), std::back_inserter(connectionsWithinBounds));
  }
}

}  // namespace flatsurf

// Instantiations of templates so implementations are generated for the linker
#include "util/instantiate.ipp"

LIBFLATSURF_INSTANTIATE_MANY_WRAPPED((LIBFLATSURF_INSTANTIATE_WITH_IMPLEMENTATION)(LIBFLATSURF_INSTANTIATE_HASH), SaddleConnectionsByLengthIterator, LIBFLATSURF_FLAT_TRIANGULATION_TYPES)
