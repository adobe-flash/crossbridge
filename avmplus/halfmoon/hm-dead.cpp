/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

/// remove a goto from a [goto]:label cluster
///
void removeGoto(GotoInstr* go) {
  assert(go->target && "goto has no target");
  if (enable_verbose)
    printf("removing dead goto %s%d\n", kInstrPrefix, go->id);
  LabelInstr* target = go->target;
  if (isAlone(go)) {
    target->preds = 0;
  } else {
    GotoInstr* N = go->next_goto;
    GotoInstr* P = go->prev_goto;
    if (target->preds == go)
      target->preds = N;
    N->prev_goto = P;
    P->next_goto = N;
  }
  go->next_goto = go->prev_goto = 0;
  go->target = 0;

  if (go->catch_blocks != NULL) {
    for (SeqRange<ExceptionEdge*> r(*go->catch_blocks); !r.empty(); r.popFront()) {
      CatchBlockInstr* cblock = r.front()->to;

      for (ExceptionEdgeRange p(cblock); !p.empty(); p.popFront()) {
        ExceptionEdge* edge = p.front();
        if (edge->from == go) {
          // dead edge
          if (edge->next_exception == edge) {
            cblock->catch_preds = NULL;
          } else {
            ExceptionEdge* N = edge->next_exception;
            ExceptionEdge* P = edge->prev_exception;
            if (cblock->catch_preds == edge) {
              cblock->catch_preds = N;
            }
            N->prev_exception = P;
            P->next_exception = N;
          }
        }
      }
    }
  }
}

/**
 * Dead code elimination.  This class implements the mark phase of an optimistic
 * two-phase algorithm.  The mark phase marks defs and instructions which are
 * directly observable or contribute to observable computations.
 *
 * A branch is marked if some marked instruction is control-dependent on it,
 * or, put another way, if it is in the reverse dominance frontier of a marked
 * instruction.
 */
class Dead {
public:
  Dead(Allocator& alloc, InstrGraph* ir)
  : alloc(alloc)
  , ir(ir)
  , defmark(alloc, ir->size())
  , instrmark(alloc, ir->size()) {
  }

  void mark();
  bool checkDead();

  /** Return true if d is marked */
  bool keepDef(Def* d) {
    return defmark.get(defId(d));
  }

  /** Return true if instr is marked */
  bool keepInstr(Instr* instr) {
    return instrmark.get(instr->id);
  }

  /** Find the nearest marked reverse dominator block */
  BlockStartInstr* findRdom(BlockEndInstr* end) {
    BlockStartInstr* b = ir->blockStart(end);
    do
      b = rdom->idom(b);
    while (rdom->hasIDom(b) && !keepInstr(b));
    return b;
  }

private:
  void markInstr(Instr*);
  void markDef(Def*);
  void markDef(const Use& u) { markDef(def(u)); }
  void markDefs(Instr*);
  void markRdf(BlockStartInstr*);
  void markEnd(BlockEndInstr*);
  void scanInstr(Instr*);
  void scanDef(Def*);

private:
  Allocator& alloc;
  InstrGraph* ir;
  BitSet defmark, instrmark;
  SeqStack<Def*> defwork;
  SeqStack<Instr*> instrwork;
  DominatorTree* rdom;
  BlockStartInstr** blockmap;
};

/// Compact the param array of a label, and the corresponding
/// arg arrays of all predecessor gotos, by removing [arg]/param
/// entries whose params are unused in the label's block.
///
bool compactLabelParams(LabelInstr* label, Dead* dead) {
  int used = 0;

  for (int i = 0, n = label->paramc; i < n; ++i) {
    Def& param = label->params[i];
    if (!dead->keepDef(&param))
      if (enable_verbose)
        printf("dead: compact %s%d %s def %d type %s is dead\n", kInstrPrefix,
            label->id, name(label), i, typeName(param));
  }

  for (int i = 0, n = label->paramc; i < n; ++i) {
    Def& param = label->params[i];
    if (dead->keepDef(&param)) {
      // def is used - move if necessary, and update used count
      if (i > used) {
        // move param def to first unused slot, and repoint its uses
        Def& new_param = label->params[used];
        assert(!new_param.isUsed());
        new (&new_param) Def(label, type(param)); // placement new
        copyUses(&param, &new_param);
        assert(!param.isUsed() && new_param.isUsed());
        // move arg use to corresponding slot in all goto preds
        for (PredRange r(label); !r.empty(); r.popFront()) {
          GotoInstr* go = r.front();
          new (&go->args[used]) Use(go, def(go->args[i])); // placement new.
          go->args[i] = 0;
        }
      }
      ++used;
    } else {
      // Def is dead: clear arg in all predecessors, and do not increment used
      for (LabelArgRange u(label, i); !u.empty(); u.popFront())
        u.front() = 0;
      for (UseRange u(param); !u.empty(); u.popFront()) {
        if (enable_verbose)
          printf("dead: unusing use in i%d %s of dead label def %d\n",
                 user(u.front())->id, name(user(u.front())), i);
        u.front() = 0;
      }
    }
  }

  // update param count if necessary
  if (used < label->paramc) {
    if (enable_verbose)
      printf("dead: compacted %s%d %s argc %d -> %d\n", kInstrPrefix, label->id,
             name(label), label->paramc, used);
    label->paramc = used;
    return true;
  }
  return false;
}

/**
 * A CondInstr's arg is considered marked if the corresponding param is marked
 * in any arm.
 */
bool argMarked(CondInstr* instr, int pos, Dead* dead) {
  for (ArmParamRange p(instr, pos); !p.empty(); p.popFront())
    if (dead->keepDef(&p.front()))
      return true;
  return false;
}

/// Compact the arg array of a cond, and the corresponding
/// param arrays in its arms, by removing arg/[param] entries
/// whose params are unused in all arms.
///
bool compactCondArgs(CondInstr* instr, Dead* dead) {
  Use* args = getArgs(instr);
  int argc = numArgs(instr);
  int used = 0;

  for (int i = 0; i < argc; ++i) {
    if (argMarked(instr, i, dead)) {
      // arg is used - move if necessary, and increment used count
      if (used < i) {
        // move arg[i] left to first unused slot
        new (&args[used]) Use(instr, def(args[i]));
        args[i] = 0;
        // in all successors, move param def to first unused slot, and repoint uses
        for (ArrayRange<ArmInstr*> r = armRange(instr); !r.empty(); r.popFront()) {
          ArmInstr& arm = *r.front();
          Def& param = arm.params[i];
          Def& new_param = arm.params[used];
          assert(!new_param.isUsed());
          new (&new_param) Def(&arm, type(param)); // placement new
          copyUses(&param, &new_param);
          assert(!param.isUsed());
        }
      }
      ++used;
    } else {
      // arg is useless - clear arg in cond, and do not increment use.
      if (enable_verbose)
        printf("dead: compact %s%d %s arg %d is dead\n", kInstrPrefix,
            instr->id, name(instr), i);
      for (ArrayRange<ArmInstr*> a = armRange(instr); !a.empty(); a.popFront())
        for (UseRange u(a.front()->params[i]); !u.empty(); u.popFront()) {
          if (enable_verbose)
            printf("dead: unusing use in i%d %s of dead arg def\n",
                   user(u.front())->id, name(user(u.front())));
          u.front() = 0;
        }
      args[i] = 0;
    }
  }

  // update arg count if necessary
  if (used < argc) {
    if (enable_verbose)
      printf("dead: compacted %s%d %s argc %d -> %d\n", kInstrPrefix, instr->id,
          name(instr), argc, used);
    instr->argc = used;
    return true;
  }
  return false;
}

/// Remove unused arg/param entries from block
/// end/start junctions.
void compactSigs(InstrGraph* ir, Dead* dead) {
  for (PostorderBlockRange r(ir); !r.empty(); r.popFront()) {
    BlockStartInstr* block = r.front();
    if (kind(block) == HR_label)
      compactLabelParams(cast<LabelInstr>(block), dead);
    BlockEndInstr* end = ir->blockEnd(block);
    InstrKind k = kind(end);
    if (k == HR_if || k == HR_switch)
      compactCondArgs((CondInstr*)end, dead);
  }
}

/**
 * Mark Instructions.
 * 1. Find the block that contains each instruction.
 * 2. Mark exit and end, if either exists.
 * 3. Transitively mark defs and instructions until there is no more work.
 *
 * Termination: Instructions and defs may only transition once each, from
 * unmarked to marked.  We only queue an instruction or def the first time it
 * is marked.
 */
void Dead::mark() {
  Allocator0 alloc0(alloc);
  blockmap = new (alloc0) BlockStartInstr*[ir->size()];
  for (EachBlock b(ir); !b.empty(); b.popFront())
    for (InstrRange i(b.front()); !i.empty(); i.popFront())
      blockmap[i.front()->id] = b.front();
  rdom = reverseDoms(alloc, ir);
  if (ir->exit)
    markInstr(ir->exit);
  if (ir->end)
    markInstr(ir->end);
  do {
    while (!instrwork.empty())
      scanInstr(instrwork.pop());
    while (!defwork.empty())
      scanDef(defwork.pop());
  } while (!instrwork.empty());

  assert(checkDead());
}

bool Dead::checkDead() {
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    for (InstrRange i(b.front()); !i.empty(); i.popFront()) {
      Instr* instr = i.front();
      if (!keepInstr(instr)) {
        if (enable_verbose)
          printf("dead: i%d\n", instr->id);
        for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront())
          assert(!keepDef(&d.front()) && "live def on dead instruction");
      } else if (enable_verbose) {
        for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront()) {
          if (!keepDef(&d.front())) {
            printf("dead: of i%d: d%d", instr->id, defId(&d.front()));
            for (d.popFront(); !d.empty(); d.popFront())
              if (!keepDef(&d.front()))
                printf(",d%d", defId(&d.front()));
            printf("\n");
            break;
          }
        }
      }
    }
  }
  return true;
}

/*
notes:
if marking an arm's def (split), mark the if, its condition, and use[i] (split input)
if marking any instr, also mark the if's it's CD on and their condition (DEAD)
  - dont need to worry about splits if we have full CD info since
    uses of splits are CD on the if
Singer: insert extra splits of constants and force them to be used (yuck)
  x = C becomes  x = C + t where t is a split zero.
could: when marking a phi, mark the CD's of the corresponding gotos, but
  dont use CD info for general instructions?
 */
void Dead::scanInstr(Instr* instr) {
  switch (kind(instr)) {
    case HR_switch:
    case HR_if: {
      CondInstr* cond = (CondInstr*) instr;
      markInstr(ir->blockStart(cond));
      markDef(cond->selector());
      break;
    }
    case HR_goto:
      markInstr(ir->blockStart((BlockEndInstr*)instr));
      break;
    case HR_label:
    case HR_arm:
      markRdf((BlockStartInstr*)instr);
      break;
    default:
      markDefs(instr);
      markInstr(blockmap[instr->id]);
      break;
  }
}

void Dead::scanDef(Def* d) {
  Instr* instr = definer(d);
  markInstr(instr);
  switch (kind(instr)) {
    case HR_label:
      // mark the corresponding arg in each incoming goto.
      for (PredRange p(cast<LabelInstr>(instr)); !p.empty(); p.popFront()) {
        GotoInstr* go = p.front();
        markDef(go->args[pos(d)]); // mark the corresponding goto input
        markInstr(go);
      }
      break;
    case HR_arm: {
      // mark the corresponding arg in the condition.
      CondInstr* cond = cast<ArmInstr>(instr)->owner;
      markDef(cond->arg(pos(d)));
      break;
    }
  }
}

/// Mark the branch instructions that block is control-dependent on,
/// which are the branches at the ends of the blocks in its reverse
/// dominance frontier.
void Dead::markRdf(BlockStartInstr* block) {
  for (SeqRange<BlockStartInstr*> r(rdom->df(block)); !r.empty(); r.popFront())
    markInstr(ir->blockEnd(r.front()));
}

void Dead::markEnd(BlockEndInstr* end) {
  BlockStartInstr* block = ir->blockStart(end);
  markInstr(block);
}

void Dead::markInstr(Instr* instr) {
  if (!instrmark.get(instr->id)) {
    instrmark.set(instr->id);
    instrwork.push(instr);
  }
}

void Dead::markDefs(Instr* instr) {
  for (ArrayRange<Use> u = useRange(instr); !u.empty(); u.popFront())
    markDef(u.front());
}

void Dead::markDef(Def* d) {
  int id = defId(d);
  if (!defmark.get(id)) {
    defmark.set(id);
    defwork.push(d);
  }
}

void clearUses(Instr* instr) {
  for (ArrayRange<Use> u = useRange(instr); !u.empty(); u.popFront())
    u.front() = 0;
}

void unlinkDeadCode(InstrGraph* ir, Dead* dead, Cleaner* clean) {
  Allocator scratch;
  BitSet keep(scratch, ir->size());
  InstrFactory factory(ir);

  // unlink unmarked instructions
  for (PostorderBlockRange b(ir); !b.empty(); b.popFront()) {
    for (InstrRange j(b.front()); !j.empty();) {
      Instr* instr = j.popBack();
      if (dead->keepInstr(instr))
        continue;
      if (isCond(instr)) {
        BlockStartInstr* rdom = dead->findRdom((BlockEndInstr*)instr);
        if (enable_verbose) {
          printf("dead: %s i%d is dead\n", name(instr), instr->id);
          printf("      nearest live rdom is i%d %s argc %d\n", rdom->id,
                 name(rdom), numDefs(rdom));
        }
        assert(numDefs(rdom) == 0);
        LabelInstr* label = kind(rdom) == HR_label ? cast<LabelInstr>(rdom) :
                            clean->ensureLabel(cast<ArmInstr>(rdom));
        GotoInstr* go = factory.newGotoStmt(label);
        ir->replaceInstr(instr, go);
        clearUses(instr);
      } else {
        if (!isBlockStart(instr) && !isBlockEnd(instr)) {
          if (enable_verbose)
            printf("dead: unlink i%d %s\n", instr->id, name(instr));
          ir->unlinkInstr(instr);
          clearUses(instr);
        } else {
          if (enable_verbose)
            printf("dead: leaving dead i%d %s\n", instr->id, name(instr));
        }
      }
    }
  }
}

/// Find instructions that are only reachable as a use of some def.
/// Any such instructions were unlinked at some point, but not fully.
/// arguably it is a bug for any such instructions to exist: had they
/// been fully unlinked at the time, this traversal would not be needed.
///
void removeDeadCode(Context* cxt, InstrGraph* ir) {
  assert(checkPruned(ir));
  if (enable_verbose) {
    printf("Before removeDeadCode\n");
    listCfg(ir->lattice.console(), ir);
  }
  assert(checkSSA(ir));

  Allocator scratch;
  Dead dead(scratch, ir);
  dead.mark();
  compactSigs(ir, &dead);
  Cleaner clean(ir);
  unlinkDeadCode(ir, &dead, &clean);
  pruneGraph(ir);

  if (enable_verbose) {
    printf("After DEAD\n");
    listCfg(ir->lattice.console(), ir);
  }

  cleanBlocks(cxt, ir, &clean);

  assert(checkPruned(ir));
  if (enable_verbose) {
    printf("After CLEAN\n");
    listCfg(ir->lattice.console(), ir);
  }
  assert(checkSSA(ir));
}

}
#endif // VMCFG_HALFMOON
