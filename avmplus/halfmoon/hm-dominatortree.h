/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DOMINATORTREE_H_
#define DOMINATORTREE_H_

namespace halfmoon {

/// The dominator tree contains a node for each block in the IR,
/// and we track the blocks immediate dominator and dominator tree-depth.
///
class DominatorTree {
public:
  DominatorTree(Allocator& alloc, InstrGraph*);

  /** True if the given block has an immediate dominator */
  bool hasIDom(BlockStartInstr* b) {
    return info_[b->blockid].idom != 0;
  }

  /** Get the immediate dominator of block b */
  BlockStartInstr* idom(BlockStartInstr* b) {
    assert(hasIDom(b));
    return info_[b->blockid].idom;
  }

  /** Return b's depth in the dominator tree.  The entry block has depth 0 */
  int depth(BlockStartInstr* b) {
    return info_[b->blockid].depth;
  }

  Seq<BlockStartInstr*>* df(BlockStartInstr* b) {
    return info_[b->blockid].df;
  }

  /** Return true if b1 dominates b2, false otherwise. */
  bool dominates(BlockStartInstr* b1, BlockStartInstr* b2) {
    do {
      if (b1 == b2)
        return true;
      b2 = info_[b2->blockid].idom;
    } while (b2);
    return false;
  }

private:
  template<class FWD, class REV> void computeGeneric(InstrGraph*);

private:
  struct BlockInfo {
    BlockStartInstr *idom;
    Seq<BlockStartInstr*> *df;
    int depth;
  };
  Allocator& dom_alloc_;
  BlockInfo* info_;

  friend DominatorTree* forwardDoms(Allocator& alloc, InstrGraph*);
  friend DominatorTree* reverseDoms(Allocator& alloc, InstrGraph*);
};

DominatorTree* forwardDoms(Allocator& alloc, InstrGraph*);
DominatorTree* reverseDoms(Allocator& alloc, InstrGraph*);

/// A Loop tree provides loop-centric attributes for each CFG block:
/// - its 'parent': every node except ir->begin has a parent.  Interior
/// nodes are loop headers, leaf nodes are blocks in loop bodies.
/// - its 'depth': depth=0 means not in a loop.  >0 indicates nesting depth.
class LoopTree {
public:
  LoopTree(Allocator& alloc, InstrGraph* ir, DominatorTree* doms)
  : alloc(alloc), ir_(ir), doms_(doms) {
    Allocator0 alloc0(alloc);
    loops_ = new (alloc0) LoopNode[ir->block_count()];
    analyze();
  }

  /// contains(h,b) = true if h contains b, i.e. b is in h's loop.
  /// contains(h,h) = true (loop headers are in their own loop)
  bool contains(BlockStartInstr* h, BlockStartInstr* block) {
    for (BlockStartInstr* b = block; b != 0; b = loop(b).h)
      if (b == h)
        return true;
    return false;
  }

  /// Recursively calculate the depth of b in the loop tree.  Nodes outside
  /// any loop have depth 0.
  int depth(BlockStartInstr* b) {
    int i = loop(b).is_header ? 1 : 0;
    BlockStartInstr* h = loop(b).h;
    return h ? i + depth(h) : i;
  }

  BlockStartInstr* parent(BlockStartInstr* b) {
    BlockStartInstr* h = loop(b).h;
    return h ? h : b != ir_->begin ? ir_->begin : 0;
  }

private:
  struct LoopNode {
    BlockStartInstr* h; // the loop that strictly contains b
    bool is_header; // true if b is itself a header (target of loop-edge)
  };

private:
  void analyze();
  void analyzeEdge(BlockStartInstr* n, BlockStartInstr* h);

  LoopNode& loop(BlockStartInstr* b) {
    return loops_[b->blockid];
  }

  BlockStartInstr* hdr(BlockStartInstr* b) {
    return loop(b).is_header ? b : loop(b).h;
  }

private:
  Allocator& alloc;
  InstrGraph* ir_;
  DominatorTree* doms_;
  LoopNode* loops_;
};


} // namespace halfmoon

#endif // DOMINATORTREE_H_
