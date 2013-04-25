/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

/// Assign block-start and block-end instructions trivially.
///
BlockStartInstr** pinBlocks(InstrGraph* ir, Allocator& alloc) {
  Allocator0 alloc0(alloc);
  BlockStartInstr** blockmap = new (alloc0) BlockStartInstr*[ir->size()];
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    blockmap[block->id] = block;
    blockmap[ir->blockEnd(block)->id] = block;
  }
  return blockmap;
}

/// Link instr into its assigned block, after the defs it uses.  Recurse
/// over use->def edges, then link once done.
void linkInstr(Instr* instr, BlockStartInstr** blockmap) {
  if (InstrGraph::isLinked(instr))
    return;
  for (ArrayRange<Use> u = useRange(instr); !u.empty(); u.popFront())
    linkInstr(definer(def(u.front())), blockmap);
  Instr* block_end = InstrGraph::blockEnd(blockmap[instr->id]);
  InstrGraph::linkBefore(block_end, instr);
  if (enable_verbose)
    printf("append %s%d to block %s%d\n", kInstrPrefix, instr->id,
           kBlockPrefix, blockmap[instr->id]->blockid);
}

/// Schedule instructions within blocks.  Blockmap contains the
/// the block assignment for each instruction.  This pass simply visits
/// instructions in reverse postorder, appending each instruction to its
/// assigned block.
///
void scheduleBlocks(InstrGraph* ir, BlockStartInstr** blockmap) {
  for (EachBlock b(ir); !b.empty(); b.popFront())
    for (ArrayRange<Use> u = useRange(InstrGraph::blockEnd(b.front())); !u.empty(); u.popFront())
      linkInstr(definer(u.front()), blockmap);
}

/// Assign instr to the highest legal block, which is the lowest block
/// of any of its inputs.
///
BlockStartInstr* hoistInstr(Instr* instr, BlockStartInstr** blockmap,
                            DominatorTree* doms, BlockStartInstr* start) {
  BlockStartInstr* block = blockmap[instr->id];
  if (block)
    return block;
  // Unlink the instruction so it can be relinked to the proper place.
  InstrGraph::unlinkInstr(instr);
  // Find the highest legal block in the dominator tree.
  BlockStartInstr* earliest = start;
  for (ArrayRange<Use> a = useRange(instr); !a.empty(); a.popFront()) {
    BlockStartInstr* arg_block = hoistInstr(definer(a.front()), blockmap, doms,
                                            start);
    if (doms->depth(arg_block) > doms->depth(earliest))
      earliest = arg_block;
  }
  if (enable_verbose)
    printf("hoist %s%d to block %s%d\n", kInstrPrefix, instr->id,
           kBlockPrefix, earliest->blockid);
  return blockmap[instr->id] = earliest;
}

/// scheduleEarly: schedule 'floaters' as high as possible
/// in the dominator tree.  A floater is an instruction that is not already
/// linked into a block.  These instructions can be placed in any
/// block dominated by all inputs and which dominates all uses.
///
/// 1. assign block-begin/end instructions to obvious blocks.
/// 2. Assign floaters to the earliest legal block.
/// 3. Link floaters to the earliest legal position in the assigned block.
///
/// The earliest legal block is the block with the lowest dominator
/// depth of any input.
///
BlockStartInstr** scheduleEarly(Allocator& alloc, InstrGraph* ir,
                                DominatorTree* doms) {
  assert(checkPruned(ir) && checkSSA(ir));
  // Assign begin/end instructions to root blocks.
  BlockStartInstr** blockmap = pinBlocks(ir, alloc);

  // 2. schedule - assign instructions to earliest possible blocks
  for (EachBlock b(ir); !b.empty(); b.popFront())
    for (ArrayRange<Use> u = useRange(ir->blockEnd(b.front())); !u.empty(); u.popFront())
      hoistInstr(definer(u.front()), blockmap, doms, ir->begin);

  // Sort instructions within each block.
  scheduleBlocks(ir, blockmap);
  assert(checkPruned(ir) && checkSSA(ir));
  return blockmap;
}

void scheduleEarly(InstrGraph* ir) {
  if (enable_verbose)
    printf("SCHEDULE EARLY\n");
  Allocator scratch;
  scheduleEarly(scratch, ir, forwardDoms(scratch, ir));
}

/// Find the lowest common dominator of a and b.
///
BlockStartInstr* findNearestDominator(DominatorTree* doms, BlockStartInstr* a,
                                      BlockStartInstr* b) {
  assert(b);
  if (!a)
    return b;
  while (doms->depth(a) > doms->depth(b))
    a = doms->idom(a);
  while (doms->depth(b) > doms->depth(a))
    b = doms->idom(b);
  while (a != b) {
    a = doms->idom(a);
    b = doms->idom(b);
  }
  assert(a && "Common dominator not found");
  return a;
}

/// Find the lowest block we can place instr, by finding the lowest common
/// dominator of each of instr's uses.
///
BlockStartInstr* sinkInstr(Instr* instr, BlockStartInstr** blockmap,
                 DominatorTree* doms) {
  BlockStartInstr* block = blockmap[instr->id];
  if (block)
    return block;
  // Unlink the instruction so it can be re-linked in its new block.
  InstrGraph::unlinkInstr(instr);
  BlockStartInstr* latest = 0;
  for (AllUsesRange u(instr); !u.empty(); u.popFront()) {
    BlockStartInstr* use_block = sinkInstr(user(u.front()), blockmap, doms);
    latest = findNearestDominator(doms, latest, use_block);
  }
  if (enable_verbose)
    printf("sink %s%d to %s%d\n", kInstrPrefix, instr->id, kBlockPrefix,
           latest->blockid);
  return blockmap[instr->id] = latest;
}

/// Schedule instructions at their latest possible points.
/// Instructions constrained by CTRL dependencies stay put.
/// Other instructions (floaters) are scheduled to the latest
/// legal point in the latest legal block.
///
void scheduleLate(InstrGraph* ir) {
  assert(checkPruned(ir) && checkSSA(ir));
  Allocator scratch;

  if (enable_verbose)
    printf("SCHEDULE LATE\n");

  // 1. Assign block-start and block-end instructions to the obvious blocks.
  BlockStartInstr** blockmap = pinBlocks(ir, scratch);

  // 2. Find the latest block we can place each floating instruction in.
  DominatorTree* doms = forwardDoms(scratch, ir);
  for (PostorderBlockRange b(ir); !b.empty(); b.popFront()) {
    for (InstrRange i(b.front()); !i.empty();) {
      Instr* instr = i.popBack();
      if (!blockmap[instr->id] && numUses(instr) == 0)
        sinkInstr(instr, blockmap, doms);
    }
    for (AllUsesRange u(b.front()); !u.empty(); u.popFront())
      sinkInstr(user(u.front()), blockmap, doms);
  }

  // 3. put instructions in order within each blocks.
  scheduleBlocks(ir, blockmap);
  assert(checkPruned(ir) && checkSSA(ir));
}

/// Find the lowest block we can place instr, by finding the lowest common
/// dominator of each of instr's uses.  Then find the shallowest loop nest
/// that's still dominated by earliest[i].
BlockStartInstr* placeInstr(Instr* instr, BlockStartInstr** blockmap,
                            BlockStartInstr** early, DominatorTree* doms,
                            LoopTree* loops) {
  BlockStartInstr* block = blockmap[instr->id];
  if (block)
    return block;
  // Unlink the instruction so it can be re-linked in its new block.
  InstrGraph::unlinkInstr(instr);
  BlockStartInstr* latest = 0;
  for (AllUsesRange u(instr); !u.empty(); u.popFront()) {
    BlockStartInstr* use_block = placeInstr(user(u.front()), blockmap, early,
                                            doms, loops);
    latest = findNearestDominator(doms, latest, use_block);
  }
  BlockStartInstr* earliest = early[instr->id];
  assert(latest && earliest && "latest or earliest not computed");
  assert(doms->dominates(earliest, latest));
  int min_depth = loops->depth(latest);
  for (BlockStartInstr* b = latest; b != earliest && min_depth > 0;) {
    b = doms->idom(b);
    int depth = loops->depth(b);
    if (depth < min_depth) {
      if (enable_verbose)
        printf("place: lift %s%d up to %s%d\n", kInstrPrefix, instr->id,
               kBlockPrefix, latest->blockid);
      min_depth = depth;
      latest = b;
    }
  }
  if (enable_verbose)
    printf("place: %s%d in %s%d\n", kInstrPrefix, instr->id, kBlockPrefix,
           latest->blockid);
  return blockmap[instr->id] = latest;
}

void scheduleMiddle(InstrGraph* ir) {
  if (enable_verbose)
    printf("SCHEDULE MIDDLE\n");
  Allocator scratch;
  DominatorTree* doms = forwardDoms(scratch, ir);
  BlockStartInstr** early = scheduleEarly(scratch, ir, doms);
  LoopTree loops(scratch, ir, doms);
  BlockStartInstr** blockmap = pinBlocks(ir, scratch);
  for (PostorderBlockRange b(ir); !b.empty(); b.popFront()) {
    for (InstrRange i(b.front()); !i.empty();) {
      Instr* instr = i.popBack();
      if (!blockmap[instr->id] && numUses(instr) == 0)
        placeInstr(instr, blockmap, early, doms, &loops);
    }
    for (AllUsesRange u(b.front()); !u.empty(); u.popFront())
      placeInstr(user(u.front()), blockmap, early, doms, &loops);
  }
  scheduleBlocks(ir, blockmap);
  assert(checkPruned(ir) && checkSSA(ir));
}

} // namespace avmplus
#endif // VMCFG_HALFMOON
