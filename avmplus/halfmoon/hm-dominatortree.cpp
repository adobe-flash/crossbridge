/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

/// Efficient sparse set for known set of unsigned ints, based
/// on Briggs and Torczon.
class SparseSet {
public:
  SparseSet(Allocator& alloc, int size)
  : sparse_(new (alloc) unsigned[size])
  , dense_(new (alloc) unsigned[size])
  , length_(0) {
    // add() can legitimately access uninitialized memory, safely,
    // so we do this to suppress valgrind errors.
    VALGRIND_MAKE_MEM_DEFINED(sparse_, sizeof(unsigned)*size);
  }

  bool contains(unsigned k) const {
    unsigned a = sparse_[k];
    return a < length_ && dense_[a] == k;
  }

  void clear() {
    length_ = 0;
  }

  typedef ArrayRange<unsigned> Range;
  Range range() const {
    return Range(dense_, length_);
  }

  /// Add k to the set, return false if it was already in the set, true
  /// if it was added by this call.
  bool add(unsigned k) {
    unsigned a = sparse_[k];
    unsigned n = length_;
    if (a >= n || dense_[a] != k) {
      sparse_[k] = n;
      dense_[n] = k;
      length_ = n + 1;
      return true;
    }
    return false;
  }

  void remove(unsigned k) {
    unsigned a = sparse_[k];
    unsigned n = length_ - 1;
    if (a <= n && dense_[a] == k) {
      unsigned e = dense_[n];
      length_ = n;
      dense_[a] = e;
      sparse_[e] = a;
    }
  }

private:
  unsigned* const sparse_;
  unsigned* const dense_;
  unsigned length_;
};

struct Block {
  BlockStartInstr* start;
  int idom; // postorder_id of immediate dominator
};

class CfgInfo {
public:
  CfgInfo(Allocator& alloc, InstrGraph* ir);
  template<class VIEW> void genericDfs(BlockStartInstr*);
  template<class VIEW> void genericDfs(InstrGraph*);
  template<class VIEW> void computeGenericDoms();
private:
  int intersect(int idom, int pred);
public:
  int max_post_id;
  int* post_ids; // postorder numbers indexed by blockid
  Block* blocks;
};

CfgInfo::CfgInfo(Allocator& alloc, InstrGraph* ir)
: max_post_id(0) {
  Allocator0 alloc0(alloc);
  int maxid = ir->block_count() + 2;
  post_ids = new (alloc0) int[maxid];
  blocks = new (alloc0) Block[maxid];
}

/// This is a reduce function.  idom is the post_id of a candidate immediate
/// dominator and pred is the post_id of any other predecessor.  Ignore
/// predecessors not yet processed.  Returns the post_id of nearest.
///
int CfgInfo::intersect(int idom, int pred) {
  Block* blocks = this->blocks;
  if (!blocks[pred].idom)
    return idom; // predecessor not processed yet
  if (!idom)
    return pred; // trivial choice
  while (idom != pred) {
    while (idom < pred)
      idom = blocks[idom].idom;
    while (pred < idom)
      pred = blocks[pred].idom;
  }
  return idom;
}

class SuccEdges {
public:
  class EdgeRange: public ArrayRange<ArmInstr*> {
  public:
    explicit EdgeRange(BlockStartInstr* b)
    : ArrayRange<ArmInstr*>(armRange((CondInstr*)end(b))) {}
  };
  class CatchEdgeRange: public CatchBlockRange {
   public:
    explicit CatchEdgeRange(BlockStartInstr* block): CatchBlockRange(block) {}
  };
  class RootRange {
  public:
    explicit RootRange(InstrGraph* ir) : begin(ir->begin) {}
    bool empty() const { return begin == 0; }
    BlockStartInstr* front() const { assert(!empty()); return begin; }
    BlockStartInstr* popFront() {
      BlockStartInstr* b = begin;
      begin = 0;
      return b;
    }
  private:
    BlockStartInstr* begin;
  };
  static bool oneEdge(BlockStartInstr* b) {
    return kind(end(b)) == HR_goto;
  }
  static BlockStartInstr* next(BlockStartInstr* b) {
    return cast<GotoInstr>(end(b))->target;
  }
  static bool manyEdges(BlockStartInstr* b) {
    InstrKind k = kind(end(b));
    return k == HR_if || k == HR_switch;
  }
  static bool hasCatchEdges(BlockStartInstr* b) {
    return InstrGraph::blockEnd(b)->catch_blocks != NULL;
  }
private:
  static BlockEndInstr* end(BlockStartInstr* b) {
    return InstrGraph::blockEnd(b);
  }
};

class PredEdges {
public:
  class EdgeRange {
  public:
    explicit EdgeRange(BlockStartInstr* b) : r(cast<LabelInstr>(b)) {}
    bool empty() const { return r.empty(); }
    BlockStartInstr* front() const { return start(r.front()); }
    BlockStartInstr* popFront() { return start(r.popFront()); }
  private:
    PredRange r;
  };
  class CatchEdgeRange: public ExceptionEdgeRange {
   public:
    explicit CatchEdgeRange(BlockStartInstr* block): ExceptionEdgeRange(cast<CatchBlockInstr>(block)) {}
    BlockStartInstr* front() {
      return InstrGraph::blockStart(ExceptionEdgeRange::front()->from);
    }
    BlockStartInstr* popFront() {
      return InstrGraph::blockStart(ExceptionEdgeRange::popFront()->from);
    }
  };
  class RootRange {
  public:
    explicit RootRange(InstrGraph* ir) : ir(ir) {
      assert((ir->exit || ir->end) && ir->exit != ir->end);
      e = ir->exit ? ir->exit : ir->end;
    }
    bool empty() const { return e == 0; }
    BlockStartInstr* front() const { assert(!empty()); return start(e); }
    BlockStartInstr* popFront() {
      BlockStartInstr* b = front();
      e = (e == ir->exit) ? ir->end : 0;
      return b;
    }
  private:
    InstrGraph* ir;
    BlockEndInstr* e;
  };
  static bool oneEdge(BlockStartInstr* b) {
    return kind(b) == HR_arm;
  }
  static BlockStartInstr* next(BlockStartInstr* b) {
    return start(cast<ArmInstr>(b)->owner);
  }
  static bool manyEdges(BlockStartInstr* b) {
    return kind(b) == HR_label;
  }
  static bool hasCatchEdges(BlockStartInstr* b) {
    return kind(b) == HR_catchblock;
  }
private:
  static BlockStartInstr* start(BlockEndInstr* e) {
    return InstrGraph::blockStart(e);
  }
};

template<class VIEW>
void CfgInfo::genericDfs(InstrGraph* ir) {
  int blockid = ir->block_count();
  post_ids[blockid] = -1;
  for (typename VIEW::RootRange r(ir); !r.empty();)
    genericDfs<VIEW>(r.popFront());
  int post_id = ++max_post_id;
  post_ids[blockid] = post_id;
  blocks[post_id].start = 0; // 0 indicates root node.
}

template<class VIEW>
void CfgInfo::genericDfs(BlockStartInstr* block) {
  int blockid = block->blockid;
  if (post_ids[blockid])
    return;
  post_ids[blockid] = -1; // mark visited
  if (VIEW::oneEdge(block)) {
    genericDfs<VIEW>(VIEW::next(block));
  } else if (VIEW::manyEdges(block)) {
    for (typename VIEW::EdgeRange s(block); !s.empty();)
      genericDfs<VIEW>(s.popFront());
  }
  if (VIEW::hasCatchEdges(block)) {
    for (typename VIEW::CatchEdgeRange s(block); !s.empty();)
      genericDfs<VIEW>(s.popFront());
  }
  int post_id = ++max_post_id;
  post_ids[blockid] = post_id;
  blocks[post_id].start = block;
}

template<class VIEW>
void CfgInfo::computeGenericDoms() {
  blocks[max_post_id].idom = max_post_id;
  bool changed;
  do {
    changed = false;
    // for all blocks in reverse postorder from start, except start:
    for (int i = max_post_id - 1; i > 0; --i) {
      // Compute idom by calling intersect() on each reachable edge.
      // Unreachable edges will have post_id == 0.
      BlockStartInstr* block = blocks[i].start;
      int idom = 0;
      if (VIEW::oneEdge(block)) {
        idom = post_ids[VIEW::next(block)->blockid];
      } else if (VIEW::manyEdges(block)) {
        for (typename VIEW::EdgeRange e(block); !e.empty();) {
          int pred_post_id = post_ids[e.popFront()->blockid];
          if (pred_post_id)
            idom = intersect(idom, pred_post_id);
        }
      } else if (!VIEW::hasCatchEdges(block)) {
        idom = max_post_id; // idom is root node
      }
      if (VIEW::hasCatchEdges(block)) {
        for (typename VIEW::CatchEdgeRange e(block); !e.empty();) {
          int pred_post_id = post_ids[e.popFront()->blockid];
          if (pred_post_id)
            idom = intersect(idom, pred_post_id);
        }
      }
      if (blocks[i].idom != idom) {
        // save the newly computed dominator
        blocks[i].idom = idom;
        changed = true;
      }
    }
  } while (changed);
}

/**
 * Compute dominators and the dominance frontier using the two-finger
 * algorithm from Cooper, Harvey, and Kennedy
 * "A Simple, Fast Dominance Algorithm."
 */
DominatorTree::DominatorTree(Allocator& dom_alloc, InstrGraph* ir)
: dom_alloc_(dom_alloc) {
  Allocator0 alloc0(dom_alloc);
  info_ = new (alloc0) BlockInfo[ir->block_count()];
}

template<class FWD, class REV>
void DominatorTree::computeGeneric(InstrGraph* ir) {
  Allocator scratch;
  CfgInfo cfg(scratch, ir);
  cfg.genericDfs<FWD>(ir);
  cfg.computeGenericDoms<REV>();
  // copy data into saved struct, compute DF and depth
  SparseSet visited(scratch, cfg.max_post_id + 1);
  for (int i = cfg.max_post_id - 1; i > 0; --i) {
    BlockStartInstr* block = cfg.blocks[i].start;
    BlockInfo& info = info_[block->blockid];
    int idom = cfg.blocks[i].idom;
    BlockStartInstr* idom_instr = cfg.blocks[idom].start;
    info.idom = idom_instr;
    info.depth = idom_instr ? info_[idom_instr->blockid].depth + 1 : 1;
    if (REV::manyEdges(block) || REV::hasCatchEdges(block)) {
      visited.clear();
    }
    if (REV::manyEdges(block)) {
      for (typename REV::EdgeRange r(block); !r.empty();) {
        BlockStartInstr* next = r.popFront();
        int next_post_id = cfg.post_ids[next->blockid];
        if (!next_post_id)
          continue; // block was not reachable
        for (int n = next_post_id; n != idom; n = cfg.blocks[n].idom) {
          if (visited.add(n)) {
            Seq<BlockStartInstr*>* &df = info_[cfg.blocks[n].start->blockid].df;
            df = cons(dom_alloc_, block, df);
          }
        }
      }
    }
    if (REV::hasCatchEdges(block)) {
      for (typename REV::CatchEdgeRange r(block); !r.empty();) {
        BlockStartInstr* next = r.popFront();
        int next_post_id = cfg.post_ids[next->blockid];
        if (!next_post_id)
          continue; // block was not reachable
        for (int n = next_post_id; n != idom; n = cfg.blocks[n].idom) {
          if (visited.add(n)) {
            Seq<BlockStartInstr*>* &df = info_[cfg.blocks[n].start->blockid].df;
            df = cons(dom_alloc_, block, df);
          }
        }
      }
    }
  }
}

DominatorTree* forwardDoms(Allocator& alloc, InstrGraph* ir) {
  DominatorTree* d = new (alloc) DominatorTree(alloc, ir);
  d->computeGeneric<SuccEdges, PredEdges>(ir);
  return d;
}

DominatorTree* reverseDoms(Allocator& alloc, InstrGraph* ir) {
  DominatorTree* d = new (alloc) DominatorTree(alloc, ir);
  d->computeGeneric<PredEdges, SuccEdges>(ir);
  return d;
}

/// Analyze each edge in the CFG.
void LoopTree::analyze() {
  for (EachBlock b(ir_); !b.empty();) {
    BlockStartInstr* block = b.popFront();
    BlockEndInstr* end = ir_->blockEnd(block);
    if (kind(end) == HR_goto)
      analyzeEdge(block, cast<GotoInstr>(end)->target);
    else if (isCond(end))
      for (ArrayRange<ArmInstr*> s = armRange((CondInstr*)end); !s.empty();)
        analyzeEdge(block, s.popFront());
  }
  if (enable_verbose) {
    for (EachBlock b(ir_); !b.empty();) {
      BlockStartInstr* block = b.popFront();
      printf("loops: B%d: depth=%d header={", block->blockid, depth(block));
      for (BlockStartInstr* h = hdr(block); h; h = loop(h).h)
        printf("B%d%s", h->blockid, loop(h).h ? "," : "");
      printf("}\n");
    }
  }
}

/* loop analysis basics

 from http://pages.cs.wisc.edu/~fischer/cs701.f08/finding.loops.html

 body = {H}
  push N onto an empty stack;
  while (stack != empty) {
    pop D from the stack;
    if (D not in body) {
      body = {D} union body;
      push each predecessor of D
        onto the stack.
    }
  }
 */

void LoopTree::analyzeEdge(BlockStartInstr* n, BlockStartInstr* h) {
  if (!doms_->dominates(h, n))
    return; // not a loop-edge
  loop(h).is_header = true;
  SeqStack<BlockStartInstr*> stack;
  stack.push(n);
  while (!stack.empty()) {
    BlockStartInstr* d = stack.pop();
    if (!contains(h, d)) {
      loop(d).h = h;
      if (kind(d) == HR_arm)
        stack.push(ir_->blockStart(cast<ArmInstr>(d)->owner));
      else if (kind(d) == HR_label)
        for (PredRange p(cast<LabelInstr>(d)); !p.empty();)
          stack.push(ir_->blockStart(p.popFront()));
    }
  }
}


} // namespace avmplus
#endif // VMCFG_HALFMOON
