/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

Cleaner::Cleaner(InstrGraph* ir)
: arm_labels_(alloc_, ir->block_count())
, ir_(ir)
, factory_(ir) {
}

/// Find and remove degenerate (1:1) goto->label and 
/// cond->arm pairs.
bool Cleaner::onePass() {
  bool changed = false;
  for (PostorderBlockRange b(ir_); !b.empty(); b.popFront())
    while (simplify(ir_->blockEnd(b.front()))) {
      changed = true;
      // return changed; // uncomment this to debug each transformation
    }
  return changed;
}

/// Simplify one block, given by "end", return true if we did work
/// and false otherwise.
bool Cleaner::simplify(BlockEndInstr* end) {
  InstrKind k = kind(end);
  if (k == HR_goto) {
    GotoInstr* go = cast<GotoInstr>(end);
    LabelInstr* target = go->target;
    if (isAlone(go))
      return joinBlocks(go, target, ir_); // Join a goto to its successor. (#3)

//    // fixme: branch hoisting is broken:
//    //   ensureLabel() inserts labels congruent to the branch arms, but they need
//    //   to be congruent with the label before the branch that we are bypassing.
//    //   fixing phis in this case needs to call translate() on each use dominated
//    //   by each arm.
    if (ir_->isEmptyBlock(target)) {
      BlockEndInstr* succ_end = ir_->blockEnd(target);
      switch (kind(succ_end)) {
//        case HR_if:
//        case HR_switch:
//          // Hoist branch out of empty block. (#4)
//          return hoistBranch(ir, go, (CondInstr*)succ_end);
//        case HR_goto:
//          // Eliminate empty block, degenerate case of hoisting branches (#2)
//          return hoistGoto(go, cast<GotoInstr>(succ_end));
      default:
        break;
      }
    }
  } else if (k == HR_switch || k == HR_if) {
    CondInstr* cond = (CondInstr*) end;
    // If all arms are redundant, merge with single successor (#1)
    LabelInstr* target = findSingleTarget(cond);
    if (target)
      return joinArms(cond, target);
    // Join a degenerate branch to its single reachable arm. (#3)
    ArmInstr* arm = getConstArm(cond);
    if (arm) {
      cond->selector() = 0; // unlink use of selector def
      return joinBlocks(cond, arm, ir_);
    }
  }
  return false;
}

/// Just a wrapper that prints verbage before joining two blocks.
///
bool Cleaner::joinBlocks(BlockEndInstr* pred, BlockStartInstr* succ,
                         InstrGraph* ir) {
  if (enable_verbose)
    printf("clean: join %s %s%d->%s%d\n", name(pred), kInstrPrefix,
           pred->id, kInstrPrefix, succ->id);
  ir->joinBlocks(pred, succ);
  return true;
}

/// Just a wrapper that prints verbage before joining a cond/arm edge.
///
bool Cleaner::joinArms(CondInstr* cond, LabelInstr* target) {
  // If all arms are redundant, merge with single successor (#1)
  if (enable_verbose)
    printf("clean: found redundant branch i%d->i%d\n", cond->id, target->id);
  assert(target->paramc == 0);
  GotoInstr* go = factory_.newGotoStmt(target);
  ir_->replaceInstr(cond, go);
  return true;
}

/// Return true if every arm of this branch is empty and ends with a goto to
/// the same label, and the label has no parameters.
/// todo: maybe arms don't have to be empty since EFFECT is explicit?
/// todo: maybe paramc can be >0 if all gotos have same signature.  Such
/// gotos might not reduce if a third unrelated goto has a different signature.
LabelInstr* Cleaner::findSingleTarget(CondInstr* branch) {
  LabelInstr* target = 0;
  for (ArrayRange<ArmInstr*> a = armRange(branch); !a.empty(); a.popFront()) {
    ArmInstr* arm = a.front();
    if (!InstrGraph::isEmptyBlock(arm))
      return 0;
    BlockEndInstr* end = InstrGraph::blockEnd(arm);
    if (kind(end) != HR_goto)
      return 0;
    GotoInstr* go = cast<GotoInstr>(end);
    if (go->target->paramc > 0)
      return 0;
    if (!target)
      target = go->target;
    else if (go->target != target)
      return 0;
  }
  return target; // All arms end with a goto to this target, with no params.
}

/// Translate defs across edges.  Use is from some instruction after a
/// label.  If the def reaching that use is from this goto's label, then
/// return the def that reaches the goto.  The use need not be in the label's
/// block, and the returned def may not be in the goto's block.
Def* Cleaner::translate(const Use& succ_go_use, GotoInstr* old_go) {
  Def* d = def(succ_go_use);
  return definer(d) == old_go->target ? def(old_go->args[pos(d)]) : d;
}

/// If we have a goto to a label followed by another goto, bypass the label
/// by creating a second goto.  i.e:
///  goto->L1;goto->L2   =>    goto->L2
bool Cleaner::hoistGoto(GotoInstr* go, GotoInstr* succ_go) {
  if (go->target == succ_go->target)
    return false;
  if (enable_verbose)
    printf("clean: goto->goto i%d->i%d->i%d\n", go->id, go->target->id,
           succ_go->id);

  int argc = succ_go->target->paramc;
  // replace go with copy of succ_go
  GotoInstr* go2 = factory_.newGotoStmt(succ_go->target, 0);
  for (int i = 0; i < argc; ++i)
    go2->args[i] = translate(succ_go->args[i], go);
  ir_->replaceInstr(go, go2);
  removeGoto(go);
  return true;
}

/// If we have a goto to a label followed by a branch, make a copy of
/// the branch to replace the goto.  for example:
///   goto->L1;C1->Arm   =>   C2->Arm;goto->L2.
/// Labels are inserted at Arm positions lazily by ensureLabel().
bool Cleaner::hoistBranch(GotoInstr* go, CondInstr* branch) {
  assert(kind(branch) == HR_if || kind(branch) == HR_switch);
  if (enable_verbose)
    printf("clean: goto->branch i%d->i%d->i%d\n", go->id, go->target->id,
           branch->id);

  int argc = branch->argc;
  // replace go with copy of branch
  Def* selector = translate(branch->selector(), go);
  if (isConst(type(selector))) {
    // Common special case: replace branch with goto.
    ArmInstr* arm1 = getConstArm(branch, type(selector));
    GotoInstr* go2 = factory_.newGotoStmt(ensureLabel(arm1), 0);
    for (int i = 0; i < argc; ++i)
      go2->args[i] = translate(branch->arg(i), go);
    ir_->replaceInstr(go, go2);
    removeGoto(go);
  } else {
    CondInstr* branch2 = kind(branch) == HR_if ?
        (CondInstr*)factory_.newIfInstr(selector, branch->argc) :
        (CondInstr*)factory_.newSwitchInstr(selector,
                                           cast<SwitchInstr>(branch)->num_cases(),
                                           argc, 0);
    for (int i = 0; i < argc; ++i)
      branch2->arg(i) = translate(branch->arg(i), go);
    ir_->replaceInstr(go, branch2);
    removeGoto(go);

    // before: go1->label1->branch1->arm1
    // after:  branch2->arm2->go2->label2
    ArrayRange<ArmInstr*> a1 = armRange(branch);
    ArrayRange<ArmInstr*> a2 = armRange(branch2);
    for (; !a1.empty(); a1.popFront(), a2.popFront()) {
      ArmInstr* arm1 = a1.front();
      ArmInstr* arm2 = a2.front();
      ir_->addBlock(arm2);
      GotoInstr* go2 = factory_.newGotoStmt(ensureLabel(arm1), 0);
      for (int i = 0; i < argc; ++i)
        go2->args[i] = arm2->params[i];
      ir_->addInstrAfter(arm2, go2);
    }
  }
  return true;
}

/// Ensure that arm is an empty block that ends with a goto to a ditinguished;
/// label that we can use for adding additional gotos.
LabelInstr* Cleaner::ensureLabel(ArmInstr* arm) {
  LabelInstr* label = arm_labels_.get(arm->blockid);
  if (!label) {
    int argc = arm->owner->argc;
    label = factory_.newLabelInstr(argc);
    arm_labels_.put(arm->blockid, label);
    copyAllUses(arm, label);
    GotoInstr* go = factory_.newGotoStmt(label, 0);
    for (int i = 0; i < argc; ++i)
      go->args[i] = arm->params[i];
    ir_->addBlockAfter(arm, label);
    ir_->addInstrAfter(arm, go);
  }
  return label;
}

/// Run the cleaner repeatedly on the IR until no more changes occur.
///
void cleanBlocks(Context* cxt, InstrGraph* ir, Cleaner* cleaner) {
  assert(checkPruned(ir) && checkSSA(ir));
  while (cleaner->onePass()) {
    pruneGraph(ir);
    if (enable_verbose) {
      cxt->out << "after clean pass\n";
      listCfg(cxt->out, ir);
    }
    assert(checkSSA(ir));
  }
}
}

#endif // VMCFG_HALFMOON
