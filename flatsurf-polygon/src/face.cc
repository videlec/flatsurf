/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2018 Alex Eskin
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

#include <boost/math/constants/constants.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <list>
#include <ostream>
#include <vector>

#include "./elementary_geometry.h"
#include "./face.h"
#include "./globals.h"
#include "./number_field.h"
#include "./params.h"
#include "./two_complex.h"
#include "./uedge.h"

using boost::numeric_cast;
using boost::math::constants::pi;
using std::endl;
using std::list;
using std::ostream;
using std::vector;

namespace polygon {
Face::Face() : Simplex() {
  tag = 'F';
  from_face = nullptr;
  parent_face_ID = -1;
}
Face::~Face() {}

Face::Face(const list<OEdge> &L) : Simplex() {
  tag = 'F';
  SetOEdgeList(L);
  from_face = nullptr;
  parent_face_ID = -1;
}

// for billiard mode
Face *Face::transform_to_original(BigPointQ v, BigPointQ &v_orig) {
  Face *f = this;
  v_orig = v;

  while (f->from_face != nullptr) {
    reflect_vector(v, f->reflection_vector, v_orig);
    f = f->from_face;
    v = v_orig;
  }
  return (f);
}

void Face::SetOEdgeList(list<OEdge> L) {
  oedges = L;

  // The following four lines are a hack for ReflectFace

  if (field_arithmetic) {
    algebraicQ zeta = algebraicQ(1, NumberField<bigrat>::F);
    algebraicQ zero_F = algebraicQ(NumberField<bigrat>::F);
    algebraicQ one_F = algebraicQ(0ul, NumberField<bigrat>::F);
    algebraicQ junk = zeta / 100;
    vector<algebraicQ> tmp2;
    tmp2.push_back(one_F + junk);
    for (size_t i = 1; i <= Params::nbr_params(); i++) {
      tmp2.push_back(zero_F);
    }

    // = { 314*zeta/100,  22*zeta.pow(2)/73 };
    alg_tQ tmp3(tmp2);
    vec = BigPointQ(tmp3); /* hack */
  }

  if (!field_arithmetic) {
    vec.cx = Point(pi<COORD>(), 1);
  }

  if (L.empty()) return;

  for (auto i = L.begin(); i != L.end(); ++i) {
    if ((*i).direction == 1) {
      (*i).ue->f0 = this;
    } else
      (*i).ue->f1 = this;
  }
}

void Face::Print(ostream &out) {
  Simplex::Print(out);

  for (auto i = oedges.begin(); i != oedges.end(); ++i) {
    out << *i;
  };
}

size_t Face::order() { return (oedges.size()); }

Point Face::barycenter() {
  Point q(0.0, 0.0), r(0.0, 0.0), t;

  for (auto i = oedges.begin(); i != oedges.end(); ++i) {
    t = (*i).vec_cx();
    q = q + t;
    r = r + q;
  }
  r = r / Point(numeric_cast<double>(order()));

  return (r);
}

Point Face::GetOffsetV0() {
  /*
                  Point q(0.0,0.0), r(0.0,0.0),t;


                  for(OEdgeIter i = oedges.begin(); i!= oedges.end() ; i++ ) {
                                  t = (*i).vec_cx();
                                  q = q+ t;
                                  r = r + q;
                  }
                  r = r/Point(order());

                  return(-r);
  */
  return (Point(0.0, 0.0));
}

bool Face::deleted() {
  if (order() == 0) return (true);

  return (false);
}

void Face::Delete() { oedges.clear(); }

void Face::Check() {
  list<OEdge>::iterator j;
  BigPointQ q, t, r;

  for (auto i = oedges.begin(); i != oedges.end(); ++i) {
    if (this != (*i).in_face()) {
      ERR_RET("check face: edge not in face");
    }

    if ((*i).tail() != (*i).next_edge()->head()) {
      printf("F%d: ", this->id());
      printf("E%d: ", (*i).id());
      printf("E%d: ", (*i).next_edge()->id());
      ERR_RET("Check face: tail edge does not match");
    }

    if ((*i).head() != (*i).prev_edge()->tail()) {
      ERR_RET("Check face:prev edge does not match");
    }

    if (field_arithmetic &&
        abs((*i).vec_cx() - (*i).vec_algt().tocomplex()) > EPSILON) {
      printf("F%d: ", this->id());
      ERR_RET("Check face: vec and vec_algt don't agree");
    }

    if (order() == 3) { /* triangles must be convex */
      if (colinear((*i).vec_cx(), (*i).next_edge()->vec_cx()) ||
          (!CCW_((*i).vec_cx(), (*i).next_edge()->vec_cx()))) {
        printf("F%d: ", this->id());
        ERR_RET("Check face: not CCW");
      }
    } else { /* order not 3 */
      if (self_intersecting()) {
        printf("F%d: ", this->id());
        ERR_RET("Check face: self intersecting");
      }
    }

    if (field_arithmetic && !((*i).ue->internal) && (*i).ue->boundary()) {
      algebraicQ direction;
      if (!(*i).vec_algt().get_direction(direction)) {
        printf("F%d: ", this->id());
        ERR_RET("Check face: boundary face not rigid");
      }
    }

    if (!(*i).ue->boundary()) {
      j = (*i).pair_edge();
      if (j == NULL_OEdgeIter) {
        printf("F%d: ", this->id());
        ERR_RET("check face: no boundary and no pair edge");
      }
      if ((*j).head() != (*i).tail() || (*j).tail() != (*i).head()) {
        printf("F%d: ", this->id());
        ERR_RET("check face:pair edges dont match");
      }
      r = (*j).vecQ();

      //	  if( (*j).head() == (*j).tail()) {
      //	      fprintf(out_f,"WARNING: non-euclidean edge\n");
      //	  }

      t = (*i).vecQ();

      r += t;

      if (abs(r.cx) > EPSILON) {
        printf("F%d: ", this->id());
        ERR_RET("Check face: pair edges vec not opposite");
      }
      if (field_arithmetic && !(r.algt == alg_tQ())) {
        printf("F%d: ", this->id());
        ERR_RET("Check face: pair edges vec_algt not opposite");
      }

      if (order() == 3) {
        if (abs((*i).vec_cx()) >=
            abs((*i).next_edge()->vec_cx()) + abs((*i).prev_edge()->vec_cx())) {
          printf("F%d: ", this->id());
          ERR_RET("triangle inequality violated");
        }
      }
    }
    t = (*i).vecQ();

    q += t;
  }
  if (abs(q.cx) > EPSILON) {
    printf("F%d: ", this->id());
    ERR_RET("check_face: total vec nonzero");
  }
  if (field_arithmetic && !(q.algt == alg_tQ())) {
    printf("F%d: ", this->id());
    ERR_RET("check_face: total algt_vec nonzero");
  }
  if (billiard_mode) {
    Face *orig_f = transform_to_original(q, r);

    auto i = oedges.begin();
    r = i->vecQ();
    r -= i->prev_edge()->vecQ();

    auto jj = orig_f->oedges.begin();
    t = jj->vecQ();
    t -= jj->prev_edge()->vecQ();

    transform_to_original(r, q);
    if (abs(t.cx - q.cx) > EPSILON) {
      std::cerr << this->id() << " original " << orig_f->id() << endl;
      std::cerr << " i = " << (*i).id() << " jj = " << (*jj).id() << endl;
      std::cerr << " i_prev " << i->prev_edge()->id()
                << " jj_prev = " << jj->prev_edge()->id() << endl;
      std::cerr << " r = " << r.cx << " t = " << t.cx << " q = " << q.cx
                << endl;

      std::cerr << " i->vec " << i->vecQ().cx << " jj->vec " << jj->vecQ().cx
                << endl;
      std::cerr << " i->prev-vec " << i->prev_edge()->vecQ().cx
                << " jj->prev-vec " << jj->prev_edge()->vecQ().cx << endl;

      ERR_RET("billiard_mode: bad edge match");
    }
  }
}

COORD Face::volume() {
  Vertex *v;
  Point p, q;

  if (order() != 3) ERR_RET("volume of non-triange");

  OEdge e1 = oedges.front();
  v = e1.tail();
  OEdge e2 = *(e1.next_edge());

  if (e2.head() != v) ERR_RET("volume:vertices don't match");

  p = -e1.vec_cx();
  q = e2.vec_cx();
  /* fix roundoff here */
  return (0.5 * abs(p) * abs(q) * sin(angle(p, q)));
}

bool Face::self_intersecting() {
  Point intersection;

  Point s1 = Point(0, 0);
  for (auto i = oedges.begin(); i != oedges.end(); ++i) {
    Point s = Point(0, 0);
    for (auto k = oedges.begin(); k != oedges.end(); ++k) {
      if (k != i && intersect_segment_interior(s1, (*i).vec_cx(), s,
                                               (*k).vec_cx(), intersection)) {
        Simplex::Print(std::cout);
        std::cout << " intersecting edges ";
        std::cout << *i;
        std::cout << " and ";
        std::cout << *k;
        std::cout << endl;
        std::cout << s1 << " " << s1 + (*i).vec_cx() << " and ";
        std::cout << s << " " << s + (*k).vec_cx() << endl;

        return (true);
      }
      s += (*k).vec_cx();
    }
    s1 += (*i).vec_cx();
  }
  return (false);
}

void Face::AddSegmentToDraw(Segment segment) {
  segments_to_draw.push_back(segment);
}

void Face::ClearSegmentsToDraw() { segments_to_draw.clear(); }
}  // namespace polygon
