/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SCHEDULERS_H_
#define SCHEDULERS_H_

namespace halfmoon {

/// "Early" Scheduling.  Place pure expression instructions in the highest
/// block in the dominator tree where all the inputs are available.
void scheduleEarly(InstrGraph*);

/// "Late" Scheduling.  Place pure expression instructions in the lowest block
/// in the dominator tree that still dominates all uses.
void scheduleLate(InstrGraph*);

/// "Middle" Scheduling.  Place instructions in the lowest block in the
/// dominator tree, among the candidate blocks with the lowest loop nesting
/// depth.  This will hoist loop invariant expressions out of loops, but
/// otherwise sink them as much as possible.
void scheduleMiddle(InstrGraph*);

} // namespace halfmoon

#endif // SCHEDULERS_H_
