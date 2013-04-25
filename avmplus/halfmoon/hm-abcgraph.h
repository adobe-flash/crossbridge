/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HALFMOON_ABCGRAPH_H_
#define HALFMOON_ABCGRAPH_H_

namespace halfmoon {
using avmplus::ExceptionHandler;
using avmplus::ExceptionHandlerTable;
using nanojit::BitSet;

/**
 * An AbcBlock marks the boundaries of a basic label in ABC.  frame_ is a
 * state vector to track the current value (Def) in each local variable,
 * scope chain slot, and operand stack slot, while building the InstrGraph.
 */
class AbcBlock {
public:
  AbcBlock(const uint8_t* pc)
  : start(pc), start_withbase(-1) {
    // Everything else is zero'd in newAbcBlock()
  }

  template<class E> FrameRange<E> startRange(E* frame, int stack_base) {
    return FrameRange < E > (frame, start_sp, start_scopep, stack_base);
  }
  FrameRange<const Type*> startTypesRange(int stack_base) {
    return startRange(start_types, stack_base);
  }

public:
  const uint8_t *start, *end; // ABC boundaries for this label.
  AbcBlock** succ_blocks; // Successor blocks from this label.
  AbcBlock** catch_blocks; // Catch blocks reachable from here (some may be null)
  int num_succ_blocks; // size of succ_blocks[] array.
  int max_catch_blocks; // size of catch_blocks[] array.
  int post_id;
  int num_preds; // Number of incoming normal and exception edges.
  bool dfs_loop; // True if target of DFS back-edge.
  bool abc_loop; // True if target of ABC back-edge.
  const Type** start_types; // Verifier-derived types at label start.
  BlockStartInstr* label; // BlockStartInstr for this AbcBlock
  int start_sp, start_scopep; // stackp_ and scope at label start
  int start_withbase;
};

/**
 * An AbcInstr is a saved copy of arguments passed by the Verifier
 * to JitWriter, for the few cases we can't or don't want to reconstruct
 * from the raw ABC.  Its a cheap way to reuse verifier specializations
 * without re-implementing them in halfmoon.
 */
struct AbcInstr {
  AbcInstr(AbcOpcode abc_op) : abc_op(abc_op), index(0) {}
  AbcInstr(AbcOpcode abc_op, uint32_t index) : abc_op(abc_op), index(index) {}
  AbcOpcode abc_op;
  uint32_t index;
};

/**
 * This class is a container for a control flow graph of AbcBlocks,
 * built by JitWriter.
 */
class AbcGraph {
public:
  AbcGraph(MethodInfo* method);

  void analyzeBranch(AbcBlock* blk, AbcOpcode abcop,
                     const uint8_t* nextpc, int32_t offset);
  void analyzeSwitch(AbcBlock* blk, const uint8_t* abc_pc,
                     const uint8_t* nextpc, int32_t default_offset,
                     uint32_t num_cases);
  AbcBlock* newAbcBlock(const uint8_t* pc);
  void analyzeEnd(AbcBlock* blk, const uint8_t* nextpc);

  /// Add a pointer in catch_blocks to each catch label reachable from
  /// this label, regardless of whether or not any instructions actually throw.
  ///
  void analyzeExceptions(AbcBlock* fm);

  AbcBlock* entry() {
    return entry_;
  }
  Allocator0& alloc0() {
    return alloc0_;
  }
  bool haveBlock(const uint8_t* pc) {
    return blockmap_.containsKey(pc);
  }
  AbcBlock* handler(int i) {
    assert(handlers_[i] == blockmap_.get(code_pos_ + table_->exceptions[i].target));
    return handlers_[i];
  }
  int handler_count() {
    return handler_count_;
  }
  const uint8_t* code_pos() {
    return code_pos_;
  }
  AbcInstr* abc_instr(const uint8_t* pc) {
    assert(abc_instrs.containsKey(pc));
    return abc_instrs.get(pc);
  }

private:
  void addAbcEdge(AbcBlock* fm, const uint8_t* target_pc, int edge_index);
  void newSuccEdges(AbcBlock* fm, int count);
  void createTryCatchBlocks();

private:
  Allocator alloc_;
  Allocator0 alloc0_;
  HashMap<const uint8_t*, AbcBlock*> blockmap_;
  HashMap<const uint8_t*, AbcInstr*> abc_instrs;
  AbcBlock* entry_;
  AbcBlock** handlers_;
  int handler_count_; // number of handlers.
  ExceptionHandlerTable* table_;
  const uint8_t* code_pos_;
  MethodInfo* method;

  friend class JitWriter;
};

/** Return true for explicit branch instructions, including jump and switch. */
inline bool isBranchOpcode(AbcOpcode abcop) {
  return abcop >= OP_ifnlt && abcop <= OP_ifstrictne;
}

/** Return true if op has no known successor instruction. */
inline bool isEndOpcode(AbcOpcode abcop) {
  return abcop == OP_returnvoid || abcop == OP_returnvalue || abcop == OP_throw;
}

inline bool isBlockEnd(AbcOpcode abcop) {
  return isBranchOpcode(abcop) || isEndOpcode(abcop) || abcop == OP_lookupswitch;
}

} // namespace avmplus
#endif // #define HALFMOON_ABCGRAPH_H_
