/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

#ifdef DEBUG
/// basic testing of range ranges and range constraints
/// should put it in a real test rig at some point
void testRangeConstraints() {

  // equality
  const RangeConstraint<int> eq0 = RangeConstraint<int>::eq(0);
  assert(eq0.implies(eq0));
  assert(!eq0.implies(!eq0));
  assert(eq0.implies(!!eq0));

  // nonequal
  const RangeConstraint<int> eq1 = RangeConstraint<int>::eq(1);
  assert(!eq1.implies(eq0));
  assert(eq1.implies(!eq0));
  assert(!(!eq1).implies(eq0));
  assert(!(!eq1).implies(!eq0));

  // range constraints
  const RangeConstraint<int> in0 = RangeConstraint<int>::in(0, 10000);
  assert(in0.implies(in0));
  assert(in0.implies(!!in0));
  assert(!in0.implies(!in0));

  assert(eq0.implies(in0));
  assert((!in0).implies(!eq0));

  // contained
  const RangeConstraint<int> in1 = RangeConstraint<int>::in(1, 10000);
  assert(in1.implies(in0));
  assert((!in0).implies(!in1));
  assert(!in0.implies(in1));
  assert(!(!in1).implies(!in0));

  assert(eq0.implies(!in1));
  assert(!(!eq0).implies(in1));

  assert(in0.r.join(in1.r) == in0.r);
  assert(in0.r.meet(in1.r) == in1.r);

  assert(in1.r.join(in0.r) == in0.r);
  assert(in1.r.meet(in0.r) == in1.r);

  // overlapping
  const RangeConstraint<int> in2 = RangeConstraint<int>::in(1, 10001);
  assert(!in0.implies(in2));
  assert(!in0.implies(!in2));
  assert(!(!in0).implies(in2));
  assert(!(!in0).implies(!in2));

  assert(in0.r.join(in2.r) == Range<int>(in0.r.x, in2.r.y));
  assert(in0.r.meet(in2.r) == Range<int>(in2.r.x, in0.r.y));

  // disjoint
  const RangeConstraint<int> in3 = RangeConstraint<int>::in(10001, 20000);
  assert(in0.implies(!in3));
  assert(!(!in0).implies(in3));

  assert(in0.r.join(in3.r) == Range<int>(in0.r.x, in3.r.y));
  assert(in0.r.meet(in3.r).empty());
}
#endif

}
#endif // #ifdef VMCFG_HALFMOON
