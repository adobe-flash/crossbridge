/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// hm-abcgraph.cpp: implementation code for ABC Control Flow Graphs
//

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

AbcGraph::AbcGraph(MethodInfo* method)
: alloc0_(alloc_), blockmap_(alloc_), abc_instrs(alloc_), entry_(0)
, handlers_(0), handler_count_(0), table_(0), method(method) {
  // create label at head of method's abc stream
  code_pos_ = method->getMethodSignature()->abc_code_start();
  entry_ = newAbcBlock(code_pos_);
  if ((table_ = method->abc_exceptions()) != 0)
    createTryCatchBlocks();
}

/// Create and add a label starting at pc to blockmap_, 
/// if not already present.
///
AbcBlock* AbcGraph::newAbcBlock(const uint8_t* pc) {
  AbcBlock* b = blockmap_.get(pc);
  if (!b) {
    if (enable_verbose)
      method->pool()->core->console << "createBlock " << int(pc - code_pos_) << "\n";
    blockmap_.put(pc, b = new (alloc0_) AbcBlock(pc));
  }
  return b;
}

/// Record an edge from one label to another, by adding
/// 'to' label to 'from' label's succ list. Create 'to'
/// label from target pc if necessary, and add it to
/// worklist if so.
///
void AbcGraph::addAbcEdge(AbcBlock* fm, const uint8_t* target_pc, int edge_index) {
  AbcBlock* to = newAbcBlock(target_pc);
  ++to->num_preds;
  if (target_pc <= fm->start)
    to->abc_loop = true;
  fm->succ_blocks[edge_index] = to;

  if (enable_verbose)
    method->pool()->core->console << "edge " << int(fm->start - code_pos_) << "->" << int(target_pc  - code_pos_) << "\n";
}

void AbcGraph::createTryCatchBlocks() {
  handler_count_ = table_->exception_count;
  if (!handler_count_)
    return;
  handlers_ = new (alloc_) AbcBlock*[handler_count_];
  for (int i = 0, n = handler_count_; i < n; ++i) {
    ExceptionHandler* handler = &table_->exceptions[i];
    newAbcBlock(code_pos_ + handler->from);
    newAbcBlock(code_pos_ + handler->to);
    handlers_[i] = newAbcBlock(code_pos_ + handler->target);
  }
}

void AbcGraph::analyzeExceptions(AbcBlock* fm) {
  assert(!fm->catch_blocks && !fm->max_catch_blocks);
  if (!table_)
    return;
  int n = table_->exception_count;
  const uint8_t* pc = fm->start;
  fm->max_catch_blocks = n;
  fm->catch_blocks = new (alloc0_) AbcBlock*[n];
  for (int i = 0; i < n; ++i) {
    ExceptionHandler* handler = &table_->exceptions[i];
    if (pc >= code_pos_ + handler->from && pc < code_pos_ + handler->to) {
      AbcBlock* catch_block = blockmap_.get(code_pos_ + handler->target);
      ++catch_block->num_preds;
      fm->catch_blocks[i] = catch_block;
    }
  }
}

void AbcGraph::analyzeBranch(AbcBlock* blk, AbcOpcode abcop, 
                             const uint8_t* nextpc, int32_t offset) {
  assert(isBranchOpcode(abcop) && abcop != OP_jump); (void)abcop;

  newSuccEdges(blk, 2);
  addAbcEdge(blk, nextpc, 0);
  addAbcEdge(blk, nextpc + offset, 1);
}

void AbcGraph::analyzeSwitch(AbcBlock* blk, const uint8_t* pc,
                             const uint8_t* nextpc, int32_t default_offset,
                             uint32_t num_cases) {
  newSuccEdges(blk, num_cases + 1);
  for (uint32_t i = 0; i < num_cases; ++i, nextpc += 3)
    addAbcEdge(blk, pc + AvmCore::readS24(nextpc), i); // case edge
  addAbcEdge(blk, pc + default_offset, num_cases); // default edge
}

// Add edge to next instr - our label ends here and
// we fall through to next label
void AbcGraph::analyzeEnd(AbcBlock* blk, const uint8_t* nextpc) {
  newSuccEdges(blk, 1);
  addAbcEdge(blk, nextpc, 0);
}

void AbcGraph::newSuccEdges(AbcBlock* block, int count) {
  assert(!block->succ_blocks && !block->num_succ_blocks);
  block->succ_blocks = new (alloc_) AbcBlock*[count];
  block->num_succ_blocks = count;
}

} // namespace avmplus
#endif // VMCFG_HALFMOON
