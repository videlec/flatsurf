/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2019 Vincent Delecroix
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

#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include <e-antic/renfxx_fwd.h>
#include <exact-real/element.hpp>
#include <exact-real/number_field.hpp>

#include <flatsurf/flat_triangulation.hpp>
#include <flatsurf/half_edge.hpp>
#include <flatsurf/saddle_connection.hpp>
#include <flatsurf/saddle_connections.hpp>
#include <flatsurf/vector.hpp>
#include <flatsurf/vector_along_triangulation.hpp>

#include "surfaces.hpp"

using namespace flatsurf;
using eantic::renf_class;
using eantic::renf_elem_class;
using std::vector;
using testing::Test;
using testing::Types;

namespace {
template <class R2>
class FlatTriangulationCombinatorialTest : public Test {};

using ExactVectors = Types<Vector<long long>, Vector<renf_elem_class>, Vector<exactreal::Element<exactreal::NumberField>>>;
TYPED_TEST_CASE(FlatTriangulationCombinatorialTest, ExactVectors);

TYPED_TEST(FlatTriangulationCombinatorialTest, FlipSquare) {
  auto square = makeSquare<TypeParam>();

  auto vertices = square->vertices();

  for (auto halfEdge : square->halfEdges()) {
    const auto vector = square->fromEdge(halfEdge);
    square->flip(halfEdge);
    EXPECT_NE(vector, square->fromEdge(halfEdge));
    square->flip(halfEdge);
    EXPECT_EQ(vector, -square->fromEdge(halfEdge));
    square->flip(halfEdge);
    square->flip(halfEdge);
    EXPECT_EQ(vector, square->fromEdge(halfEdge));

    // a square (torus) has only a single vertex so it won't change; in general
    // it should not change, however, the representatives attached to a vertex
    // are currently not properly updated: https://github.com/flatsurf/flatsurf/issues/100
    EXPECT_EQ(vertices, square->vertices());
  }
}
}  // namespace

#include "main.hpp"
