/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_DEAD_H_
#define HM_DEAD_H_

namespace halfmoon {

/// Find and remove stragglers: instructions that are using a live
/// instruction, and thus reachable in a def->use traversal, but not
/// live themselves (not reachable in a use->def traversal).
///
void removeDeadCode(Context*, InstrGraph*);

/// Remove a goto from its label's list of predecessors
void removeGoto(GotoInstr* go);

/// Unlink and set all uses in instr to 0 so this instruction isn't a straggler.
void clearUses(Instr* instr);
}

#endif
