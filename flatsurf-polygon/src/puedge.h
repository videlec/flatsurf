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

#ifndef FLATSURF_PUEDGE_H
#define FLATSURF_PUEDGE_H

#include "./defs.h"
#include "./oedge.h"
#include "./psimplex.h"
#include "./uedge.h"

namespace polygon {
class PUEdge : public PSimplex {
 public:
  PUEdge(UEdge*, Point, size_t);
  PUEdge(OEdge&, Point, size_t);

  void Draw(my_ostream&, COORD) override;

  UEdge* s;
  Simplex* sp() override;
};
}  // namespace polygon

#endif  // FLATSURF_PUEDGE_H
