/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HALFMOON_DISPATCH_H
#define HALFMOON_DISPATCH_H

namespace halfmoon {

/// KindAdapter implements a default do function for every opcode by calling
/// the adapter function A.do_default().  Subclasses shadow any function they
/// want to implement.  When used with do_instr<A>(), the subclass function
/// is called.  For example:
///
/// class MyKindAdapter : public KindAdapter<MyInstrAdapter, int> {
/// public:
///   int do_add(BinaryStmt* instr);
/// };
///
/// KindAdapter uses "static polymorphism", via the curiously recurring
/// template pattern (CRTP):
///
template<class SELF_CLASS, class RETURN_TYPE>
class KindAdapter {
public:
  typedef RETURN_TYPE dispatch_return_type;

#include "generated/KindAdapter_methods.hh"
};

/// do_instr() is a template function for doing something specific based
/// on opcode.  INSTR_ADAPTER must have a do_xxx for each opcode named HR_xxx,
/// plus a do_default function for opcodes that no adapter has any special
/// code for.  (basically instructions not yet implemented).
///
template<class KIND_ADAPTER>
typename KIND_ADAPTER::dispatch_return_type do_instr(KIND_ADAPTER* a, Instr* instr) {
  switch (kind(instr)) {
#include "generated/KindAdapter_cases.hh"
  default:
    assert(false && "unknown opcode");
    return a->do_default(instr);
  }
}

/// ShapeAdapter implements a default do function for every shape by calling
/// the adapter function A.do_default().  Subclasses shadow any function they
/// want to implement.  When used with do_shape<A>(), the subclass function
/// is called.  For example:
///
/// class MyShapeAdapter : public ShapeAdapter<MyShapeAdapter, int> {
/// public:
///   int do_BinaryStmt(BinaryStmt* instr);
/// };
///
/// ShapeAdapter uses "static polymorphism", via the curiously recurring
/// template pattern (CRTP):
///
template<class SELF_CLASS, class RETURN_TYPE>
class ShapeAdapter {
public:
  typedef RETURN_TYPE dispatch_return_type;

#include "generated/ShapeAdapter_methods.hh"
};

/// do_shape() is a template function for doing something specific based
/// on shape.  SHAPE_ADAPTER must have a do_xxx for each shape named xxx,
/// plus a do_default function for shapes that no adapter has any special
/// code for.
///
template<class SHAPE_ADAPTER>
typename SHAPE_ADAPTER::dispatch_return_type do_shape(SHAPE_ADAPTER* a, Instr* instr) {
  switch (shape(instr)) {
#include "generated/ShapeAdapter_cases.hh"
  default:
    assert(false && "unknown shape");
    return a->do_default(instr);
  }
}

} // namespace halfmoon

#endif // HALFMOON_DISPATCH_H
