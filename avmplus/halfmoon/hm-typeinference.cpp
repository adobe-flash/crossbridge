/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON
#include "profiler/profiler-main.h"

namespace halfmoon {

/**
 * WorkList used for propagateTypes().  fixme: this should use SeqStack<T>.
 */
template<class T> struct WorkList {
  Allocator& alloc;
  Seq<T>* work;
  Seq<T>* end;
  Seq<T>* free;
  BitSet members;
  WorkList(Allocator& alloc, int max_id) :
    alloc(alloc), work(0), end(0), free(0), members(alloc, max_id) {
  }

  bool empty() {
    return !work;
  }

  Seq<T> *newnode(T item) {
    members.set(item->id);
    Seq<T>* n = free;
    if (n) {
      free = n->tail;
      n->head = item;
    } else {
      n = new (alloc) Seq<T> (item);
    }
    return n;
  }

  void add(T item) {
    if (members.get(item->id))
      return;
    Seq<T>* n = newnode(item);
    n->tail = 0;
    if (end)
      end->tail = n;
    else
      work = n;
    end = n;
  }

  T pop() {
    assert(!empty());
    Seq<T>* n = work;
    work = n->tail;
    if (!work)
      end = 0;
    n->tail = free;
    free = n;
    T item = n->head;
    members.clear(item->id);
    return item;
  }

  void dump(PrintWriter& console) {
#ifdef AVMPLUSE_VERBOSE
    console << "[";
    for (Seq<T>* n = work; n; n = n->tail)
      console << kInstrPrefix << n->head->id << (n->tail ? "," : "");
    console << "]";
#else
    (void) console;
#endif
  }
};

/**
 * propagateTypes() - Sparse Conditional Constant Propagation
 *
 * Traditional SCCP a worklist of blocks and instructions, visits blocks
 * when discovered to be reachable, and visits instructions once when their
 * block becomes reachable and again when their inputs change.
 *
 * The classic algorithm works on constants.  Our works on types, which
 * includes constants.
 *
 * Instr-based SCCP only needs one worklist.  Only instructions with uses
 * ever get evaluated, and thus 0-input instructions like start and constants
 * must be pushed on the worklist initially.  Side effects are explicit values,
 * so statements are added to the worklist once all inputs (including effect)
 * become defined.
 *
 * for N in G
 *   reset-types(N)
 * work = {begin, constants}
 * while work not empty
 *   remove N from work
 *   compute-types(N)
 *   if changed
 *     work += uses(N)
 *
 * TODO: we will only profit here at loop headers since forward
 * flow optimizations are applied during graph-building.
 *
 * TODO: remove more null checks by either hoisting or loop peeling;
 * loop peeling is simpler because it does not reorder side effects.
 *
 * TODO: we could refine this by only pushing the uses of the
 * result which changed.  We would have to copy the types[] array
 * before calling computeTypes(), computeTypes() would no longer need
 * to compute the changed boolean.
 */

class SCCP {
public:
  SCCP(Allocator& alloc, InstrGraph* ir)
  : alloc(alloc)
  , ir(ir)
  , ssawork(alloc, ir->size())
  , cfgwork()
  , mark(alloc, ir->size())
  , analyzer(ir) {
    Allocator0 alloc0(alloc);
    eval_counts = new (alloc0) int[ir->size()];
  }

  void init();
  void analyze();

private:
  void analyzeBlock(BlockStartInstr*);
  void analyzeSSA(Instr*);
  void analyzeBranch(BlockEndInstr*);
  void addInstrUsers(Instr*);
  void addBlock(BlockStartInstr*);

private:
  Allocator& alloc;
  InstrGraph* ir;
  WorkList<Instr*> ssawork;
  SeqStack<BlockStartInstr*> cfgwork;
  BitSet mark;
  const Type** old_types;
  TypeAnalyzer analyzer;
  int* eval_counts; // count how many times each instruction is evaluated.
};

/**
 * Enque the block.  If it is already visited, just enqueu it.
 * Otherwise, mark it reached and enque all the instructions in the block.
 */
void SCCP::addBlock(BlockStartInstr* block) {
  if (!mark.get(block->id))
    cfgwork.push(block);
  else
    ssawork.add(block);
}

/**
 * helper - add the users of the Defs of a given
 * instruction to the passed worklist. 
 * 
 * This encapsulates several nontrivial pieces of logic:
 * 1. Skip use instructions that haven't been reached yet.
 * 2. Skip over dead arms of constant conditionals.
 * 3. Jump the call-site gap between block delims.
 *    (goto -> label, cond -> arm)
 * 4. When reaching a block the first time, mark all its instructions reached
 *    *and* enque them, because they could have been previously skipped.
 */
void SCCP::addInstrUsers(Instr* instr) {
  for (AllUsesRange u(instr); !u.empty(); u.popFront()) {
    Instr* use_instr = user(u.front());
    if (mark.get(use_instr->id))
      ssawork.add(use_instr);
  }
}

void SCCP::init() {
  // Reset types of all non-root instrs.
  // dev: also find capacity for temp def array
  int max_defs = 1;
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    if (!hasRootDefs(instr))
      resetTypes(instr);
    int num_defs = numDefs(instr);
    if (num_defs > max_defs) max_defs = num_defs;
  }

  // dev: we use this to hold saved types for reality check
  old_types = new (alloc) const Type*[max_defs];
}

/**
 * Instrumentation to troubleshoot poor asymptotic complexity.
 * print out the # of times we visit each instruction.  Optimal counts
 * are H*d(g)+k, where d(g) is the loop connectedness of the graph, and
 * k is probably 1 and H depends on the height of the type lattice; both
 * should be low constants.  (handwave).  An eval count in the 100's or
 * 1000's means something is terribly wrong.
 */
void printCounts(InstrGraph* ir, int* eval_counts) {
  printf("SCCP eval counts\n");
  for (int i = 0, n = ir->size(); i < n; ++i) {
    if (eval_counts[i] > 0)
      printf("%d i%d\n", eval_counts[i], i);
  }
  printf("SCCP end counts\n");
}

void SCCP::analyze() {
  // Queue the first block to start iteration.
  cfgwork.push(ir->begin);
  while (!cfgwork.empty()) {
    while (!cfgwork.empty())
      analyzeBlock(cfgwork.pop());
    while (!ssawork.empty())
      analyzeSSA(ssawork.pop());
  }
  // TODO own cmdline flag? something less intrusive
  // if ((enable_verbose || enable_printir) && ir->size() > 1000)
  //  printCounts(ir, eval_counts);
}

void SCCP::analyzeBlock(BlockStartInstr* block) {
  // mark each instruction in the block.
  for (InstrRange i(block); !i.empty(); i.popFront())
    mark.set(i.front()->id);
  // evaluate each instruction and queue all reachable users.
  for (InstrRange i(block); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    eval_counts[instr->id]++;
    analyzer.computeTypes(instr);
    addInstrUsers(instr);
  }
  analyzeBranch(ir->blockEnd(block));
}

void SCCP::analyzeBranch(BlockEndInstr* end) {
  switch (kind(end)) {
    case HR_goto:
      addBlock(cast<GotoInstr>(end)->target);
      break;
    case HR_if:
    case HR_switch: {
      CondInstr* cond = (CondInstr*)end;
      ArmInstr* arm = getConstArm(cond);
      if (arm)
        addBlock(arm);
      else
        for (ArrayRange<ArmInstr*> a = armRange(cond); !a.empty();)
          addBlock(a.popFront());
      break;
    }
  }
  if (end->catch_blocks != NULL) {
    for (CatchBlockRange r(end); !r.empty();)
      addBlock(r.popFront());
  }
}

void SCCP::analyzeSSA(Instr* instr) {
  if (numDefs(instr) == 0) {
    if (isBlockEnd(instr))
      analyzeBranch((BlockEndInstr*)instr);
    return;
  }
  // dev: save current types for later check
  int i = 0;
  for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront(), ++i)
    old_types[i] = type(d.front());
  // compute types
  eval_counts[instr->id]++;
  analyzer.computeTypes(instr);
  // dev: check computation result
  bool changed2 = false;
  i = 0;
  for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront(), ++i) {
    assert(subtypeof(old_types[i], type(d.front())) && "illegal type narrowing");
    changed2 |= *type(d.front()) != *old_types[i];
  }
  if (changed2) {
    if (enable_typecheck) {
      printInstr(instr);
    }
    addInstrUsers(instr);
  }
}

/**
 * propagate types through an InstrGraph, starting with root defs 
 * in start and constant instructions.
 *
 * Note that per SCCP, types in dead arms of constant conditionals 
 * are set to UN.
 *
 * Algorithm outline: we maintain two structures: a worklist 
 * of instructions to evaluate, and a set of instructions 
 * marked reachable.
 *
 * Initialize:
 * 1. Mark the instructions in the root block as reachable.
 * 2. For each instruction, if it's a root (no uses), then
 *    enque each reachable use.  Otherwise set all defs to _
 *    (bottom).
 * Propagate:
 * 1. while worklist not empty, deque instruction I
 * 2. evaluate I's output types using TypeAnalyzer
 * 3. If any types changed, assert(old-T <: new-T) and 
 *    enque reachable uses.
 * 4. If I is a branch, mark and enque reachable arms.
 */
void propagateTypes(InstrGraph* ir) {
  Allocator scratch;
  SCCP sccp(scratch, ir);
  sccp.init();
  sccp.analyze();
  assert(checkTypes(ir, false));
}

/**
 * Calculate the types of an IR graph with respect to the
 * specified argument types, rather than the graph start
 * instruction's stated param types.
 *
 * Calculated types are left in the graph, with the exception
 * that the graph's param types are restored after the calculation.
 * Calculated output types may be retrieved from the graph's return
 * instruction. Note: it's the caller's responsibility to copy
 * calculated results to a permanent location: they will be
 * overwritten the next time this function is called on the graph.
 * 
 * TODO check compatibility of arg types with param types.
 * TODO use type range instead of array, check arg count
 */
void propagateTypes(InstrGraph* ir, const Type** arg_types) {
  Allocator scratch;
  StartInstr* start = cast<StartInstr> (ir->begin);
  int paramc = start->paramc;
  Def* params = start->params;

  const Type** save_types = new (scratch) const Type*[paramc];
  for (int i = 0; i < paramc; i++) {
    save_types[i] = type(params[i]);
    setType(&params[i], arg_types[i]);
  }
  
  propagateTypes(ir);

  for (int i = 0; i < paramc; i++) {
    setType(&params[i], save_types[i]);
  }
}

/**
 * Propagate types through a fat instruction subgraph,
 * starting from the input types of an instance.
 * 
 * Note: propagated type are left in place in the subgraph;
 * caller must copy them out to a permanent location.
 */
void propagateTypes(Instr* fat_instr) {
  assert(hasSubgraph(fat_instr));
  InstrGraph* ir = subgraph(fat_instr);
  assert(kind(ir->begin) == HR_template);
  assert(numDefs(ir->begin) == numUses(fat_instr));
  Allocator scratch;
  propagateTypes(ir, getUseTypes(scratch, fat_instr));
}

}
#endif // VMCFG_HALFMOON
