/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_CLEANER_H_
#define HM_CLEANER_H_

namespace halfmoon {

/// Cleaner
/// in CFG postorder, until fixed point:
/// #1 eliminate redundant branch: edges to same label & same goto args:
///    replace with single goto
/// #2 eliminate empty block
///    [L1;goto->L2] - change all L1's gotos to L2, fix up shuffled args
/// #3 coalesce blocks
///    eliminate Label with 1 goto or arm chosen by const condition
///    done - see onePass() calling joinBlocks()
/// #4 hoist branches out of empty blocks
///    [goto->L1;cond a1,a2] => [cond]
///    replace arms with labels
///
class Cleaner {
public:
  Cleaner(InstrGraph* ir);
  bool onePass();
  LabelInstr* ensureLabel(ArmInstr* arm);

private:
  bool simplify(BlockEndInstr* block);
  bool hoistBranch(GotoInstr* go, CondInstr* branch);
  bool hoistGoto(GotoInstr* go, GotoInstr* succ_go);
  bool joinBlocks(BlockEndInstr* pred, BlockStartInstr* succ, InstrGraph* ir);
  bool joinArms(CondInstr* cond, LabelInstr* target);
  Def* translate(const Use&, GotoInstr* go);
  LabelInstr* findSingleTarget(CondInstr* branch);

private:
  Allocator alloc_;
  HashMap<int, LabelInstr*> arm_labels_; // one label for each arm that gained goto's
  InstrGraph* ir_;
  InstrFactory factory_;
};

/// Simplify the control flow graph as much as possible by
/// removing constant conditions, removing dead blocks, bypassing
/// redundant jumps, etc.
void cleanBlocks(Context* cxt, InstrGraph* ir, Cleaner*);

}

#endif
