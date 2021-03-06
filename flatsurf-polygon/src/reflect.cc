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

#include <list>

#include "./elementary_geometry.h"
#include "./face.h"
#include "./globals.h"
#include "./my_ostream.h"
#include "./psimplex.h"
#include "./two_complex.h"
#include "./uedge.h"

using std::endl;
using std::flush;
using std::list;

namespace polygon {
extern bool closure;

void TwoComplex::SetupReflect() {
  int k;

  for (auto i = faces.begin(); i != faces.end(); ++i) {
    for (auto j = (*i)->oedges.begin(); j != (*i)->oedges.end(); ++j) {
      if (!(*j).ue->boundary()) {
        (*j).ue->internal = true;
        (*j).from_edge = (*j).pair_edge();
      }
    }
  }

  k = 0;
  for (auto i = uedges.begin(); i != uedges.end(); ++i) {
    (*i)->label = k++;
  }
}

Face *TwoComplex::ReflectFace(UEdge *e, Face *f) {
  Face *new_f;
  Vertex *new_v, *first_v, *last_v;
  UEdge *tmp;
  BigPointQ tmp_vec;
  list<OEdge> tmp_edge_list;

  //  f = e->boundary_face();

  first_v = AddVertex(UNDEFINED);
  last_v = first_v;

  typedef list<OEdge>::reverse_iterator OEdgeRIter;

  for (OEdgeRIter i = f->oedges.rbegin(); i != f->oedges.rend(); ++i) {
    /* possible clean up? */
    OEdgeRIter j = i;
    ++j;
    if (j == f->oedges.rend()) { /* i.e we are at first element */
      new_v = first_v;
    } else {
      new_v = AddVertex(UNDEFINED);
    }
    reflect_vector((*i).vecQ(), e->ue_vecQ, tmp_vec);

    tmp = AddUEdge(UNDEFINED, last_v, new_v, -tmp_vec);
    tmp->label = (*i).ue->label;

    OEdge *new_edge = new OEdge(tmp, 1);

    if ((*i).ue->internal) {
      new_edge->ue->internal = true;
      new_edge->from_edge = (*i).from_edge;
    }
    tmp_edge_list.insert(tmp_edge_list.end(), *new_edge);
    delete new_edge;
    last_v = new_v;
  }

  new_f = AddFace(UNDEFINED, tmp_edge_list);
  reflect_vector(f->vec, e->ue_vecQ, new_f->vec);

  for (auto i = new_f->oedges.begin(); i != new_f->oedges.end(); ++i) {
    tmp = FindMatchingEdge((*i).ue);
    if (tmp != nullptr) {
      IdentifyEdges(tmp, (*i).ue);
    }
  }

  if (billiard_mode) {
    new_f->from_face = f;
    new_f->reflection_vector = e->ue_vecQ;
  }

  return (new_f);
}

UEdge *TwoComplex::IdentifyEdges(UEdge *ue0, UEdge *ue1) {
  int r;
  Face *f1;
  list<OEdge>::iterator e0, e1;

  if (ue0 == ue1) ERR_RET("identify_edges: same edge");

  //    f0 = ue0->boundary_face();
  f1 = ue1->boundary_face();

  e0 = ue0->boundary_edge();
  e1 = ue1->boundary_edge();

  r = test_congruent(e0, e1);

  if (r == CONG_SAME) {
    IdentifyVertices(e0->head(), e1->head());
    IdentifyVertices(e0->tail(), e1->tail());
    e1->direction = e0->direction;
    e1->ue = e0->ue;

  } else if (r == CONG_REVERSE) {
    IdentifyVertices(e0->head(), e1->tail());
    IdentifyVertices(e0->tail(), e1->head());
    e1->direction = -e0->direction;
    e1->ue = e0->ue;

  }

  else
    ERR_RET("identify edges: edges not congruent");

  ue0->set_null_face(f1);

  /* make all the pointers pointing at e1 point at e0 */

  for (auto i = vertices.begin(); i != vertices.end(); ++i) {
    if ((*i)->e == ue1) {
      (*i)->e = ue0;
    }
  }

  ue1->v0->order--; /*idenify vertices adds orders... */
  ue1->v1->order--;

  ue1->f0 = nullptr;
  ue1->f1 = nullptr;

  return (ue0);
}

Vertex *TwoComplex::IdentifyVertices(Vertex *v0, Vertex *v1) {
  if (v0 == v1) return (v0);

  for (auto j = uedges.begin(); j != uedges.end(); ++j) {
    if ((*j)->v0 == v1) (*j)->v0 = v0;

    if ((*j)->v1 == v1) (*j)->v1 = v0;
  }
  v0->order = v0->order + v1->order;
  v1->Delete();
  //  later remove v1 from the vertices list...

  return (v0);
}

UEdge *TwoComplex::FindMatchingEdge(UEdge *ue) {
  UEdge *candidate;
  list<OEdge>::iterator e, oc;
  Face *f;
  BigPointQ tmp;

  f = ue->boundary_face();
  e = ue->boundary_edge();

  /* fix later */

  if (!ue->internal) {
    reflect_vector(f->vec, e->vecQ(), tmp);
  }
  for (auto i = uedges.begin(); i != uedges.end(); ++i) {
    candidate = (*i);

    if (candidate->label != ue->label) continue;

    if (candidate == ue) continue;
    if (!candidate->boundary()) continue;
    oc = candidate->boundary_edge();

    if (test_congruent(oc, e) == CONG_FALSE) continue;
    if (!field_arithmetic && !ue->internal &&
        Dist2(candidate->boundary_face()->vec.cx, tmp.cx) > EPSILON * EPSILON)
      continue;
    if (field_arithmetic && !ue->internal) {
      //	 candidate->boundary_face()->vec.Check();
      if (candidate->boundary_face()->vec.algt != tmp.algt) {
        continue;
      }
    }
    if (ue->internal && candidate->internal &&
        candidate->this_edge(candidate->boundary_face())
                ->from_edge->pair_edge() !=
            ue->this_edge(ue->boundary_face())->from_edge)
      continue;

    return (candidate);
  }
  return nullptr;
}

void TwoComplex::CompleteSurface() {
  list<UEdge *>::iterator i;

  if (verbose >= 1) {
    std::cout << "In CompleteSurface" << endl;
    std::cout << flush;
  }

  CheckAllFaces();

  SetupReflect();

  while (1) {
    for (i = uedges.begin(); i != uedges.end(); ++i) {
      if ((*i)->boundary()) {
        break;
      }
    }
    if (i == uedges.end()) break;
    if (verbose >= 1) {
      fprintf(out_f, "Reflecting E%d\n", (*i)->id());
    }

    ReflectFace((*i), (*i)->boundary_face());
    Expunge();
    CheckAllFaces();

    CompleteInternal(*i);
  }
  CheckAllFaces();
}

void TwoComplex::SetInternalEdges() {
  for (auto i = faces.begin(); i != faces.end(); ++i) {
    for (auto k = (*i)->oedges.begin(); k != (*i)->oedges.end(); ++k) {
      if ((*k).ue->internal) {
        (*k).from_edge = (*k).pair_edge();
      }
    }
  }
}

void TwoComplex::CompleteInternal(UEdge *e) {
  list<UEdge *>::iterator k;

  int j = 0;

  char buf[1000];

  while (1) {
    for (k = uedges.begin(); k != uedges.end(); ++k) {
      if ((*k)->boundary() && (*k)->internal) {
        break;
      }
    }
    if (k == uedges.end()) break;

    ReflectFace(e,
                (*k)->this_edge((*k)->boundary_face())->from_edge->in_face());

    j++;
    sprintf(buf, "movie%d", j);
    my_ostream *movie_stream;
    movie_stream = new my_ostream(buf);
    for (auto d : dl) delete d;
    dl.clear();
    make_pcomplexes();
    NewDraw(*movie_stream);
    movie_stream->close();
    delete movie_stream;

    Expunge();

    CheckAllFaces();
  }
  SetInternalEdges();
}
}  // namespace polygon
