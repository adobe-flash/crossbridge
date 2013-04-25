/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using nanojit::BitSet;

//
// Some rules to check on the whole IR:
//
// Verifier peekType() sites:
// * nextname, nextvalue, hasnext RHS must be int.
// * hasnext2 obj & ctr cant be the same value.  ctr must be int.
// * lookupswitch input must be int
// * newobj names must be String
// * qname with runtime-name must be String
// * multiname with rtns must be Namespace
//
// Type lattice stuff:
// * No arg can be UN
// * type parameter to coerce, istype, cannot be const.
// * no value arg can be EFFECT
// * effect arg must be EFFECT
//
// InstrGraph correctness:
// * every EFFECT result must be used exactly once, except stop.
// * value args cannot be EFFECT or UN.
// * no type result can be null.
// * every effect input must be EFFECT or UN.
// * every phi must have same argc as corresponding label
// * if a phi has 2+ inputs that are the same pred instr, then the value
// must also be the same.
//
// CFG correctness
// * each def must dominate all uses
// * start instr must dominate all blocks
// * stop instr must post-dominate all blocks
// * exactly one start block with no predecessors containing the start instr.
// * exactly one end block with no successors containing the end instr.
// * All defs must dominate all uses.
// * Every instr in the SON must be reachable from the CFG.
//

#ifdef DEBUG

bool checkResultTypes(Instr* instr, bool check_model) {
  if (!hasOutputSignature(instr))
    return true;
  SigRange s = outputSigRange(instr);
  ArrayRange<Def> d = defRange(instr);
  for (; !d.empty(); d.popFront(), s.popFront()) {
    const Type* def_type = type(d.front());
    const Type* sig_type = s.front();
    assert(def_type && "Type not defined");

    if (!subtypeof(def_type, sig_type)) {
      assert(false);
    }

    if (check_model) {
      assert(submodelof(def_type, sig_type));
    }
  }

  return true;
}

/// print instr ids of owners of all uses in list 
/// where of used def is at given position in definer
///
void printScannedUsers(int defpos, Seq<Use*>* list) {
  printf("[");
  for (SeqRange<Use*> u(list); !u.empty(); u.popFront()) {
    Use* use = u.front();
    if (pos(def(*use)) == defpos)
      printf("%s%d,", kInstrPrefix, user(*use)->id);
  }
  printf("]\n");
}

/// print instr ids of owners of all uses linked to a given def
///
void printLinkedUsers(Def* d) {
  printf("[");
  for (UseRange u(*d); !u.empty(); u.popFront())
    printf("%s%d,", kInstrPrefix, user(u.front())->id);
  printf("]\n");
}
void printLinkedUsers(Def& d) {
  return printLinkedUsers(&d);
}

bool checkStartEnd(InstrGraph* ir) {
  // regress/bug_515935.abc fails because of this assert, which is for(;;)
  // abcbuilder optimizes away the loop condition into a true
  // and removes the return in the loop exit block. 
  // TODO: add cktimeout instruction at backedges if interrupts enabled
  // otherwise assert fail is ok.
  assert(ir->begin && (ir->end || ir->exit) && "Start or stop is missing");
  return true;
}

bool checkOneEnd(InstrGraph* ir, Instr* end, InstrKind k) {
  assert(!end || kind(end) == k);
  if (!end)
    return true;
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    Instr* block_end = ir->blockEnd(b.front());
    assert(block_end == end || kind(block_end) != k);
  }
  return true;
}

bool checkUses(InstrGraph* ir) {
  assert(checkStartEnd(ir));
  Allocator scratch;
  Allocator0 scratch0(scratch);

  // uses is a list of uses per instruction, by id
  Seq<Use*>** uses = new (scratch0) Seq<Use*>*[ir->size()];
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    for (ArrayRange<Use> u = useRange(i.front()); !u.empty(); u.popFront()) {
      int id = definerId(u.front());
      uses[id] = cons(scratch, &u.front(), uses[id]);
    }
  }

  // for each instr in ir, reconcile each def's linked and scanned uses
  int** use_counts = new (scratch0) int*[ir->size()];
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr *instr = i.front();
    int id = instr->id;

    if (instr == ir->end) {
      assert(uses[id] == 0);
      continue;
    } else if (kind(instr) == HR_start || kind(instr) == HR_template) {
      assert(instr == ir->begin);
    }

    // Count the number of uses of each result of i.
    int num_defs = numDefs(instr);
    use_counts[id] = new (scratch0) int[num_defs];
    for (SeqRange<Use*> u(uses[id]); !u.empty(); u.popFront())
    use_counts[id][pos(def(*u.front()))]++;

    int pos = 0;
    for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront(), ++pos) {
      // make sure each linear result has exactly one use.
      Def& def = d.front();
      int use_count = count(UseRange(d.front()));

      // compare scanned uses to incrementally maintained use list.
      if (use_counts[id][pos] != use_count) {
        printf("use count mismatch %s%d.%d\n", kInstrPrefix, id, pos);
        printf("scanned: "); printScannedUsers(pos, uses[id]);
        printf("linked:  "); printLinkedUsers(def);
        assert(!"Use count mismatch");
      }

      // Ensure each linear result has only one use.
      //assert(!isLinear(type(def)) || use_count == 1);
    }
  }

  return true;
}

//
// checkTypes
//

class TypeChecker: public KindAdapter<TypeChecker, bool> {
public:
  explicit TypeChecker(bool check_model = false) : check_model(check_model) {}

public: // do_instr interface
  bool do_default(Instr*);
  bool do_goto(GotoInstr*) { return true; } // Anything goes.

private:
  void report(int arg_number, const Use& arg, const Type* sig_type,
              const char *relation);
  void fail();

private:
  bool check_model;
};

void TypeChecker::report(int arg_number, const Use& arg, const Type* sig_type,
                         const char *relation) {
  printf("typecheck failed: %s %s%d use %d: %s not %s %s\n",
         name(user(arg)), kInstrPrefix, user(arg)->id,
         arg_number, typeName(arg), relation, typeName(sig_type));
  fail();
}

void TypeChecker::fail() {
  /* breakpoint here to debug */
  assert(false && "typecheck failed");
}

bool TypeChecker::do_default(Instr* instr) {
  // All inputs must have a type and not UN.
  for (ArrayRange<Use> u = useRange(instr); !u.empty(); u.popFront()) {
    const Use& use = u.front();
    assert(&use != NULL && "input was null");
    assert(type(use) /*&& !isBottom(type(use))*/);
  }
  if (hasInputSignature(instr)) {
    // Check types from signature.
    SigRange sig = inputSigRange(instr);
    ArrayRange<Use> u = useRange(instr);
    for (int i = 0; !u.empty(); u.popFront(), sig.popFront(), ++i) {
      const Type* t = type(u.front());
      const Type* constraint = sig.front();
      if (!subtypeof(t, constraint)) {
        avmplus::AvmCore* core = avmplus::AvmCore::getActiveCore();
        printCompactInstr(core->console, instr, false);
        report(i, u.front(), constraint, "subtype of");
      } else if (check_model && !submodelof(t, constraint)) {
        avmplus::AvmCore* core = avmplus::AvmCore::getActiveCore();
        printCompactInstr(core->console, instr, false);
        report(i, u.front(), constraint, "submodel of");
      }
    }
  } else {
    assert(numUses(instr) == 0 && "missing input signature");
  }

  return true;
}

bool checkTypes(Instr* instr, bool check_model) {
  TypeChecker a(check_model);
  return do_instr(&a, instr);
}

bool checkTypes(InstrGraph* g, bool check_model) {
  Allocator scratch;
  assert(checkUses(g));
  for (AllInstrRange iter(g); !iter.empty(); iter.popFront())
    if (!checkTypes(iter.front(), check_model))
      return false;
  return true;
}

bool AbcBuilder::checkFrame(Def* frame[], int sp, int scopep) {
  assert(sp >= stack_base_ - 1);
  assert(scopep >= scope_base_ - 1);
  assert(scopep < stack_base_);
  FrameRange<Def*> d = frameRange(frame);
  for (; !d.empty(); d.popFront())
    assert(d.front());
  return true;
}

/// Make sure list1 and list2 are disjoint; both are linked, and
/// neither is a member of the other's list.
///
bool InstrGraph::checkDisjoint(Instr* list1, Instr* list2) {
  return checkDisjoint(list1, InstrRange(list2, list2->prev_))
      && checkDisjoint(list2, InstrRange(list1, list1->prev_));
}

bool InstrGraph::checkDisjoint(Instr* instr, const InstrRange& list) {
  return find(InstrRange(list), instr).empty();
}

bool checkPruned(InstrGraph* ir) {
  Allocator scratch;
  BitSet marked(scratch, ir->size());
  for (AllInstrRange i(ir); !i.empty(); i.popFront())
    marked.set(i.front()->id);
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    for (AllUsesRange u(instr); !u.empty(); u.popFront())
      assert(marked.get(user(u.front())->id) && "user not linked");
    if (kind(instr) == HR_label)
      for (PredRange p(cast<LabelInstr>(instr)); !p.empty(); p.popFront())
        assert(marked.get(p.front()->id) && "goto not linked");
    if (kind(instr) == HR_catchblock) {
      CatchBlockInstr* cblock = cast<CatchBlockInstr>(instr);
      for (ExceptionEdgeRange p(cblock); !p.empty(); p.popFront()) {
        assert(p.front()->from->catch_blocks != NULL);
        bool found = false;
        for (CatchBlockRange r(p.front()->from); !r.empty(); r.popFront()) {
          if (r.front() == cblock) found = true;
        }
        assert(found);
      }
    }
    if (isBlockEnd(instr)) {
      BlockEndInstr* end = (BlockEndInstr*)instr;
      if (end->catch_blocks != NULL) {
        for (CatchBlockRange r(end); !r.empty(); r.popFront()) {
          bool found = false;
          for (ExceptionEdgeRange p(r.front()); !p.empty(); p.popFront()) {
            if (p.front()->from == end) found = true;
          }
          if (!found) {
            printf("missing exception back edge: i%d -> i%d", end->id, r.front()->id);
          }
          assert(found);
        }
      }
    }
  }
  if (ir->end)
    assert(marked.get(ir->end->id) && "end not linked");
  if (ir->exit)
    assert(marked.get(ir->exit->id) && "exit not linked");
  return true;
}

typedef BlockStartInstr BSI;

/**
 * Check the SSA property that every use is dominated by its def.
 * In a preorder traversal of dominator tree:
 * 1. mark this block
 * 2. check each instruction I in current block:
 *    if any def reaching a use in I doesn't dominate this block
 *    (i.e. isn't marked), then fail.
 * 3. visit dominator children
 * 4. unmark this block
 */
void checkSSABlock(BSI* block, Seq<BSI*>** children, BitSet* visited, BSI** blockmap) {
  int blockid = block->blockid;
  visited->set(blockid);
  for (InstrRange i(block); !i.empty();) {
    Instr* instr = i.popFront();
    for (ArrayRange<Use> u = useRange(instr); !u.empty();) {
      Instr* def_instr = definer(def(u.popFront()));
      BlockStartInstr* def_block = blockmap[def_instr->id];
      assert(def_block && "definer not linked");
      assert(visited->get(def_block->blockid) && "def doesn't dominate use");
    }
  }
  for (SeqRange<BSI*> r(children[blockid]); !r.empty(); )
    checkSSABlock(r.popFront(), children, visited, blockmap);
  visited->clear(blockid);
}

bool checkSSA(InstrGraph* ir) {
  Allocator scratch;
  Allocator0 scratch0(scratch);
  BSI** blockmap = new (scratch0) BSI*[ir->size()];
  Seq<BSI*>** children = new (scratch0) Seq<BSI*>*[ir->block_count()];
  BitSet visited(scratch, ir->block_count());
  DominatorTree* doms = forwardDoms(scratch, ir);
  for (EachBlock b(ir); !b.empty(); ) {
    BlockStartInstr* block = b.popFront();
    if (doms->hasIDom(block)) {
      // remember all dominator children of each block
      BSI *idom = doms->idom(block);
      children[idom->blockid] = cons(scratch, block, children[idom->blockid]);
    }
    // remember the block that each instruction is inside of
    for (InstrRange i(block); !i.empty();)
      blockmap[i.popFront()->id] = block;
  }
  checkSSABlock(ir->begin, children, &visited, blockmap);
  return true;
}

/// LirBlock is a basic block of LIR instructions.
///
struct LirBlock {
  typedef HashMap<LIns*, LirBlock*> map_t;
  typedef Seq<LirBlock*> seq_t;

  LirBlock(LIns* last, LirBlock* next, int linear_id)
  : first_ins(0)
  , last_ins(last)
  , linear_id(linear_id)
  , post_id(0)
  , num_succs(0)
  , succs(0)
  , preds(0)
  , loop_pred(0)
  , prev(0)
  , next(next)
  , next_rpo(0)
  , idom(0) {
    if (next)
      next->prev = this;
  }

  void allocSuccs(int count, Allocator& a) {
    num_succs = count;
    succs = new (a) LirBlock*[count];
  }

  void setSucc(int i, LirBlock* s, Allocator& a) {
    assert(s && i >= 0 && i < num_succs);
    succs[i] = s;
    s->preds = new (a) seq_t(this, s->preds);
    if (linear_id >= s->linear_id &&
        (!s->loop_pred || linear_id > s->loop_pred->linear_id))
      s->loop_pred = this;
  }

public:
  LIns *first_ins, *last_ins; // LIns range for this block.
  int linear_id;              // Linear block number.  Entry is lowest.
  int post_id;                // Post-order block number.  Entry is highest.
  int num_succs;
  LirBlock** succs;       // Successor blocks arranged according to last_ins.
  seq_t *preds;           // List of predecessors in arbitrary order.
  LirBlock* loop_pred;    // Latest predecessor after this block.
  LirBlock *prev, *next;  // Links to form linear list of blocks.
  LirBlock* next_rpo;     // Next block in reverse postorder.
  LirBlock* idom;         // Immediate dominator.
};

/// Perform a depth first search over successor edges starting from the
/// entry block.  Assign a postorder id (post_id) to each block, and sort
/// the blocks into reverse postorder.
///
class DepthFirstSearch {
public:
  DepthFirstSearch(LirBlock* b)
  : next_post_id(0)
  , list(0) {
    dfs(b);
  }

  void dfs(LirBlock* b) {
    b->post_id = -1;
    for (int i = 0, n = b->num_succs; i < n; ++i) {
      LirBlock* s = b->succs[i];
      if (s->post_id == 0)
        dfs(s);
    }
    b->post_id = ++next_post_id;
    b->next_rpo = list;
    list = b;
  }

private:
  int next_post_id;
  LirBlock* list;
};

/// Compute dominator tree using Cooper, Harvey, and Kennedy's "A Simple,
/// Fast, Dominance Algorithm."
///
void computeDoms(LirBlock* entry) {
  Allocator scratch;
  Allocator0 scratch0(scratch);

  DepthFirstSearch dfs(entry);
  entry->idom = entry;
  bool changed;
  do {
    changed = false;
    // for all blocks in reverse postorder except entry:
    for (LirBlock* b = entry->next_rpo; b; b = b->next_rpo) {
      LirBlock* idom = 0;
      for (SeqRange<LirBlock*> i(b->preds); !i.empty(); i.popFront()) {
        LirBlock* pred = i.front();
        if (!pred->idom)
          continue;
        if (!idom) {
          idom = pred; // Pick first already-processed predecessor.
          continue;
        }
        // Intersect pred and idom
        while (pred != idom) {
          while (pred->post_id < idom->post_id)
            pred = pred->idom;
          while (idom->post_id < pred->post_id)
            idom = idom->idom;
        }
      }
      if (idom != b->idom) {
        b->idom = idom;
        changed = true;
      }
    }
  } while (changed);
  entry->idom = 0;
}

/// LInsStatus is our cheat: this is how we keep track of LIR instructions.
enum LInsStatus {
  kNeverUsed,
  kUsed,
  kRetired
};

LInsStatus getStatus(LIns* ins) {
  return ins->isInAr() ? (LInsStatus) ins->getArIndex() : kNeverUsed;
}

void setStatus(LIns* ins, LInsStatus status) {
  ins->setArIndex((uint32_t) status);
}

void resetStatus(LIns* last_ins) {
  LirReader r(last_ins);
  for (LIns* ins = r.read(); !ins->isop(LIR_start); ins = r.read())
    ins->clearArIndex();
}

bool isLirLive(LIns* ins) {
  return ins->isop(LIR_lived) || ins->isop(LIR_livei)
#ifdef VMCFG_64BIT 
      || ins->isop(LIR_liveq)
#endif
  ;
}

class LirCfg {
  typedef HashMap<LIns*, LirBlock*> insmap_t; // map ins->block
public:
  LirCfg(Fragment* fragment)
  : printer(fragment->lirbuf->printer)
  , last_ins(fragment->lastIns)
  , num_blocks(0)
  , first_block(0)
  , last_block(0)
  , error(false)
  {
    Allocator scratch;
    LirBlock::map_t blockmap(scratch);

    // Find blocks.
    int num_ins = 0;
    LirBlock* blocks = last_block = new (alloc) LirBlock(last_ins, 0, num_blocks++);
    LirReader r(last_ins);
    for (LIns* ins = r.read(); !ins->isop(LIR_start); ins = r.read()) {
      assert(getStatus(ins) == kNeverUsed);
      num_ins++;
      blocks->first_ins = ins;
      LIns* prev = r.peek();
      if (ins->isop(LIR_label) || !prev || prev->isBranch() || prev->isRet()) {
        blockmap.put(ins, blocks); // remember first instr in block.
        if (prev)
          blocks = new (alloc) LirBlock(prev, blocks, num_blocks++);
      }
    }

    // Reverse linear_id's so lower numbers are first.
    for (LirBlock* b = blocks; b; b = b->next)
      b->linear_id = num_blocks - b->linear_id - 1;

    // Create insmap now that we know how many instructions there are.
    insmap = new (alloc) insmap_t(alloc, num_ins / 8 + 1);

    // Connect blocks and populate insmap.
    for (LirBlock* b = blocks; b; b = b->next) {
      LIns* last = b->last_ins;
      if (last->isop(LIR_jtbl)) {
        uint32_t num_cases = last->getTableSize();
        b->allocSuccs(num_cases, alloc);
        for (uint32_t i = 0; i < num_cases; ++i)
          b->setSucc(i, blockmap.get(last->getTarget(i)), alloc);
      } else if (last->isop(LIR_j)) {
        b->allocSuccs(1, alloc);
        b->setSucc(0, blockmap.get(last->getTarget()), alloc);
      } else if (last->isBranch()) {
        b->allocSuccs(2, alloc);
        b->setSucc(0, b->next, alloc);
        b->setSucc(1, blockmap.get(last->getTarget()), alloc);
      } else if (last->isRet() || !b->next) {
        // no successors
      } else {
        // plain instruction; succ is fall-through block.
        b->allocSuccs(1, alloc);
        b->setSucc(0, b->next, alloc);
      }

      // Scan each instruction and populate insmap.
      LirReader r2(last);
      LIns* ins;
      do {
        insmap->put(ins = r2.read(), b);
      } while (ins != b->first_ins);
    }
    first_block = blocks;
  }

  ~LirCfg() {
    resetStatus(last_ins); // Reset status-tracking parts of each LIns.
  }

  /// Return true if b1 dominates b2.
  ///
  bool dominates(LirBlock* b1, LirBlock* b2) {
    for (LirBlock* b = b2; b; b = b->idom)
      if (b == b1)
        return true;
    return false;
  }

  /// Return true if the edge from b1->b2 is a linear backwards edge (i.e.
  /// from a higher block number to a lower block number in linear order.
  ///
  bool isBackEdge(LirBlock* b1, LirBlock* b2) {
    return b1->linear_id >= b2->linear_id;
  }

  /// Return true if b1 is after b2 in linear order.
  ///
  bool isAfter(LirBlock* b1, LirBlock* b2) {
    return b1->linear_id > b2->linear_id;
  }

  /// if ins is a LIR_alloc or an addp(allocp, x), then return the alloc ins.
  ///
  LIns* getAllocIns(LIns* ins) {
    if (ins->isop(LIR_allocp))
      return ins;
    if (ins->isop(LIR_addp)) {
      LIns* alloc = getAllocIns(ins->oprnd1());
      if (alloc)
        return alloc;
    }
    return 0;
  }

  /// Check for missing live hints.
  ///
  void checkLive(LIns* def_ins, LIns* use_ins) {
    LirBlock* def_block = block(def_ins);
    LirBlock* use_block = block(use_ins);
    for (LirBlock* b = use_block; b && b != def_block; b = b->prev) {
      LirBlock* pred = b->loop_pred;
      if (pred && !isAfter(use_block, pred)) {
        // pred->b is a loop edge and use_block is not after
        // pred.  Therefore def_ins's live range isn't long enough.
        if (printer) {
          RefBuf rb;
          InsBuf ib;
          printf("LIR Error: Definition %s in B%d needs a LIR_live after B%d "
                 "because Use %s in B%d is before back-edge B%d->B%d.\n",
                 printer->formatRef(&rb, def_ins, false), def_block->linear_id,
                 pred->linear_id,
                 printer->formatIns(&ib, use_ins), use_block->linear_id,
                 pred->linear_id, b->linear_id);
        }
        setError();
      }
    }
    LIns* alloc;
    if (use_ins->isCall() &&
        (alloc = getAllocIns(def_ins)) != NULL &&
        getStatus(alloc) == kNeverUsed) {
      // The latest use of this alloc is a call, which means the LIR_allocp
      // needed a live hint to ensure it was kept allocated across the call.
      if (printer) {
        RefBuf rb;
        InsBuf ib;
        printf("LIR Error: alloc %s in B%d needs a LIR_live after %s in B%d "
               "to extend its live range across the call.\n",
               printer->formatRef(&rb, alloc, false), blockid(alloc),
               printer->formatIns(&ib, use_ins), use_block->linear_id);
      }
      setError();
    }
  }

  void checkRetired(LIns* def_ins, LIns* use_ins) {
    switch (getStatus(def_ins)) {
    case kNeverUsed:
      checkLive(def_ins, use_ins);
      setStatus(def_ins, kUsed);
      return;
    case kUsed:
      return;
    }
    if (printer) {
      RefBuf rb;
      InsBuf ib;
      printf("LIR Error: Definition %s in B%d must come before Use: %s in B%d\n",
             printer->formatRef(&rb, def_ins, false), blockid(def_ins),
             printer->formatIns(&ib, use_ins), blockid(use_ins));
    }
    setError();
  }

  bool isReachable(LirBlock* b) {
    return b == first_block || b->idom;
  }

  LirBlock* block(LIns* ins) {
    return insmap->get(ins);
  }

  int blockid(LIns* ins) {
    return block(ins)->linear_id;
  }

  /// Check that block(def) dominates block(use) if the latter is reachable.
  ///
  void checkDominates(LIns* def_ins, LIns* use_ins) {
    LirBlock* use_block = block(use_ins);
    if (!isReachable(use_block))
      return; // Ignore unreachable uses.
    if (dominates(block(def_ins), use_block))
      return;
    if (printer) {
      RefBuf rb;
      InsBuf ib;
      printf("LIR Error: Definition %s in B%d does not dominate Use: %s in B%d\n",
             printer->formatRef(&rb, def_ins, false), blockid(def_ins),
             printer->formatIns(&ib, use_ins), blockid(use_ins));
    }
    setError();
  }

  /// Set a breakpoint here to debug errors.
  ///
  void setError() {
    error = true;
  }

  void use(LIns* use_ins, LIns* def_ins) {
    assert(insmap->containsKey(def_ins) && insmap->containsKey(use_ins));
    checkDominates(def_ins, use_ins);
    checkRetired(def_ins, use_ins);
  }

  void def(LIns* def_ins) {
    setStatus(def_ins, kRetired);
  }

  // Print the LIR Control Flow Graph.
  void print(AvmLogControl* logc) {
    if (!printer)
      return;
    for (LirBlock* b = first_block; b; b = b->next) {
      RefBuf rb1, rb2;
      logc->printf("B%d [%s - %s]", b->linear_id,
             printer->formatRef(&rb1, b->first_ins, false),
             printer->formatRef(&rb2, b->last_ins, false));
      if (b->idom)
        logc->printf(" idom=B%d", b->idom->linear_id);
      else if (!isReachable(b))
        logc->printf(" unreachable");

      logc->printf(" preds [ ");
      for (SeqRange<LirBlock*> i(b->preds); !i.empty(); i.popFront()) {
        LirBlock* pred = i.front();
        logc->printf(" B%d ", pred->linear_id);
      }
      logc->printf("]");
      logc->printf(" succs [ ");
      for (int s = 0; s < b->num_succs; s++) {
        LirBlock* succ = b->succs[s];
        logc->printf(" B%d ", succ->linear_id);
      }
      logc->printf("]");
      logc->printf("\n");

      LIns* last = b->last_ins;
      LirReader r2(last);
      ReverseLister rl(&r2, alloc, printer, logc, "Initial LIR");

      // Scan each instruction and populate insmap.
      LIns* ins;
      do {
        ins = rl.read();
      } while (ins != b->first_ins);
      rl.finish();

    }
  }

public:
  Allocator alloc;
  LInsPrinter* printer;
  LIns* last_ins;
  int num_blocks;
  LirBlock* first_block;  // First block in linear order.
  LirBlock* last_block;   // Last block in linear order.
  insmap_t* insmap;       // Map of LIns -> LirBlock.
  bool error;
};

// fixme dont use halfmoon-specific enable_verbose
// todo Test checkLir() on sanities, brightspot, past bugs.
// todo move checkLir() to nanojit
bool checkLir(Fragment* fragment, AvmLogControl* logc) {
  Allocator scratch;
  LirCfg cfg(fragment);
  computeDoms(cfg.first_block);

  if (enable_verbose)
    cfg.print(logc);

  // Go through LIR instructions in reverse, checking each one.
  for (LirBlock* b = cfg.last_block; b; b = b->prev) {
    LirReader r(b->last_ins);
    LIns *ins;
    do {
      ins = r.read();
      switch (repKinds[ins->opcode()]) {
      case LRK_Op1:
      case LRK_Ld:
        cfg.use(ins, ins->oprnd1());
        break;
      case LRK_Op2:
      case LRK_St:
        if (!ins->isop(LIR_j)) // oprnd1 of LIR_j is null (condition).
          cfg.use(ins, ins->oprnd1());
        if (!ins->isBranch()) // oprnd2 of jt/jf is the label.
          cfg.use(ins, ins->oprnd2());
        break;
      case LRK_Op3:
        cfg.use(ins, ins->oprnd1());
        cfg.use(ins, ins->oprnd2());
        if (!ins->isBranch()) // oprnd3 of jov is the label.
          cfg.use(ins, ins->oprnd3());
        break;
      case LRK_C:
        for (int i = 0, n = ins->argc(); i < n; ++i)
          cfg.use(ins, ins->arg(i));
        break;
      }
      cfg.def(ins);
    } while (ins != b->first_ins);
  }

  assert(!cfg.error); // assert while cfg still in scope so we can debug.
  return !cfg.error;
}

#endif // debug opt-check
} // namespace avmplus
#endif // VMCFG_HALFMOON
