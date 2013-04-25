/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

InstrGraph::InstrGraph(InstrFactory* factory, InfoManager* infos)
: lattice(factory->lattice())
, begin(0), end(0), exit(0)
, infos_(infos)
, alloc_(factory->alloc())
, instr_count_(0)
, block_count_(0)
, def_count_(0) {
}

Instr* finishAdding(InstrGraph* ir, Instr* instr) {
  if (enable_typecheck)
    printCompactInstr(ir->lattice.console(), instr, false);
  TypeAnalyzer(ir).computeTypes(instr);
  if (enable_verbose)
    printInstr(ir->lattice.console(), instr);
  return instr;
}

Instr* InstrGraph::addInstrAfter(Instr* pos, Instr* instr) {
  assert(!isBlockStart(instr));
  assignId(instr);
  linkAfter(pos, instr);
  return finishAdding(this, instr);
}

Instr* InstrGraph::addInstrBefore(Instr* pos, Instr* instr) {
  assert(!isBlockStart(instr));
  assignId(instr);
  linkBefore(pos, instr);
  return finishAdding(this, instr);
}

void InstrGraph::assignBlockId(BlockStartInstr* block) {
  assert(block->blockid == -1);
  block->blockid = block_count_++;
  assignId(block);
}

Instr* InstrGraph::addBlock(BlockStartInstr* instr) {
  assignBlockId(instr);
  instr->prev_ = instr->next_ = instr; // link it to itself.
  return finishAdding(this, instr);
}

Instr* InstrGraph::addBlockAfter(BlockStartInstr* instr, LabelInstr* label) {
  assignBlockId(label);
  Instr* end = instr->prev_;
  Instr* first = instr->next_;
  label->prev_ = end;
  label->next_ = first;
  first->prev_ = end->next_ = label;
  instr->next_ = instr->prev_ = instr;
  return finishAdding(this, label);
}

/// Returns the return statement for the IR.
///
StopInstr* InstrGraph::returnStmt() const {
  assert(end && kind(end) == HR_return);
  return end;
}

StopInstr* InstrGraph::throwStmt() const {
  assert(!exit || kind(exit) == HR_throw);
  return exit;
}

/** Iteratively find the start of instr's block. */
BlockStartInstr* InstrGraph::findBlockStart(Instr* instr) {
  assert(instr->isLinked());
  if (isBlockEnd(instr))
    return blockStart((BlockEndInstr*)instr);
  while (!isBlockStart(instr))
    instr = instr->prev_;
  return (BlockStartInstr*)instr;
}

/** Iteratively find the end of instr's block. */
BlockEndInstr* InstrGraph::findBlockEnd(Instr* instr) {
  assert(instr->isLinked());
  if (isBlockStart(instr))
    return blockEnd((BlockStartInstr*)instr);
  while (!isBlockEnd(instr))
    instr = instr->next_;
  return (BlockEndInstr*)instr;
}

/// Split a list of instructions into two lists.  Afterwards, the second
/// list begins with the instruction after victim.
///
void InstrGraph::splitList(const InstrRange& list, Instr *victim) {
  Instr* pos = victim->next_;
  unlinkInstr(victim);
  assert(!pos->isAlone());
  assert(list.back()->next_ == list.front() && pos->checkLinks());

  printTerseInstrList(list, this, "before split: list");
  Instr* front = list.front();
  Instr* back = list.back();
  Instr* P = pos->prev_;

  // after_list
  pos->prev_ = back;
  back->next_ = pos;

  // before_list
  front->prev_ = P;
  P->next_ = front;

  assert(checkDisjoint(pos, front));
  assert(pos->isLinked());

  printTerseInstrList(front, this, "after split: first");
  printTerseInstrList(pos, this, "after split: pos");
}

/// Link a list in before pos.
/// =>P<=>pos<=  +  list<=...=>list_end  =  =>P<=>list<=...=>list_end<=>pos<=
///
void InstrGraph::insertList(const InstrRange& list, Instr* pos) {
  if (list.empty())
    return;
  Instr* P = pos->prev_;
  Instr* list_front = list.front();
  Instr* list_back = list.back();

  // Link in list before pos
  list_front->prev_ = P;
  P->next_ = list_front;
  pos->prev_ = list_back;
  list_back->next_ = pos;
}

/// Link a list in after pos.
/// =>pos<=>N<=  +  list<=...=>list_end  =  =>pos<=>list<=...=>list_end<=>N<=
///
void InstrGraph::appendList(Instr* pos, const InstrRange& list) {
  Instr* new_front = pos->next_;
  Instr* list_front = list.front();
  Instr* list_end = list.back();

  // Link in list after pos.
  pos->next_ = list_front;
  list_front->prev_ = pos;
  list_end->next_ = new_front;
  new_front->prev_ = list_end;
}

/// Replace outer_stmt in outer_ir with start_block and return_block.  They
/// could be the same list, so handle that simple case here.  If they are
/// different lists, split the block containing callsite.
/// Middle blocks don't need any attention here because they remain
/// connected via start_block and end_block.
///
void InstrGraph::inlineBlocks(Instr* callsite, InstrRange start_block,
                              InstrRange return_block) {
  if (start_block.front() == return_block.front()) {
    start_block.unlinkFront();
    start_block.unlinkBack();
    replaceInstr(callsite, start_block);
  } else {
    start_block.unlinkFront();
    return_block.unlinkBack();
    replaceInstr2(callsite, start_block, return_block);
  }
}

bool InstrGraph::isEmptyBlock(BlockStartInstr* block) {
  return block->next_ == block->prev_;
}

/// Given a block instruction (isBlockStart() returns true),
/// return the corresponding back_ (jump, branch, etc) instruction.
///
BlockEndInstr* InstrGraph::blockEnd(BlockStartInstr* block) {
  if (!hasBlockEnd(block)) {
    assert(false);
  }
  //assert(hasBlockEnd(block));
  return (BlockEndInstr*)block->prev_;
}

bool InstrGraph::hasBlockEnd(BlockStartInstr* block) {
  assert(isBlockStart(block));
  return isBlockEnd(block->prev_);
}

/// Find the block owner, given a branch instruction.
///
BlockStartInstr* InstrGraph::blockStart(BlockEndInstr* branch) {
  assert(isBlockStart(branch->next_));
  return (BlockStartInstr*)branch->next_;
}

/// Replace pos with list.
/// =>P<=>pos<=>N<=  +  list  =  =>P<=>list<=>N<=
///
void InstrGraph::replaceInstr(Instr* pos, const InstrRange& list) {
  assert(pos->isLinked() && checkDisjoint(pos, list));
  insertList(list, pos);
  unlinkInstr(pos);
}

/// Replace pos with two lists.
///
void InstrGraph::replaceInstr2(Instr* pos, const InstrRange& before_list,
                               const InstrRange& after_list) {
  // pos must have unique predecessor and successors.
  assert(
      pos && pos->isLinked() && pos->next_ != pos && pos->prev_ != pos->next_);
  assert(checkDisjoint(pos, before_list));
  assert(checkDisjoint(pos, after_list));
  assert(before_list.empty() || checkDisjoint(before_list.front(), after_list));
  assert(after_list.empty() || checkDisjoint(after_list.front(), before_list));
  assert(isBlockEnd(before_list.back()) && isBlockStart(after_list.front()));

  InstrRange block = blockRange(pos);

  printTerseInstrList(block, this, "before inlining: pos's block");
  printTerseInstrList(before_list, this, "before inlining: before_list");
  printTerseInstrList(after_list, this, "before inlining: after_list");

  Instr* P = pos->prev_;
  Instr* N = pos->next_;
  splitList(block, pos);
  appendList(P, before_list);
  insertList(after_list, N);

  printTerseInstrList(block.front(), this, "after inlining: before_list");
  printTerseInstrList(after_list.front(), this, "after inlining: after_list");
}

/// Join a pair of blocks. 
///
/// pred_end is the block end delimiter of the predecessor block.
/// succ_start is the block start delimiter of the successor block.
///
/// 1. Repoint all uses of succ_start's params to the defs of the
/// corresponding args in pred_end.
///
/// 2. Unlink succ_start from its block. This has two effects:
/// first it allows us to merge the successor block's contents
/// and end delimiter into the predecessor block in (3); second, 
/// it makes succ_start an orphan. See notes below.
///
/// 3. In the predecessor block, replace pred_end with the 
/// contents and end delimiter of the successor block.
///
/// Note: this operation does *not* remove succ_start from any pending 
/// iterators over block start delimiters, such as EachBlock. Callers 
/// are responsible for detecting (using isLinked) and handling orphaned 
/// block starts following this operation, and for ensuring that they 
/// become unreachable. 
///
void InstrGraph::joinBlocks(BlockEndInstr* pred_end, BlockStartInstr* succ_start) {
  assert(isDelimPair(pred_end, succ_start) && "pred_end and succ_start are unrelated");
  int param_count = numDefs(succ_start);
  // For each succ_start param, redirect all uses to the def specified 
  // by pred_end's corresponding arg, then clear the arg.
  for (int i = 0; i < param_count; ++i) {
    Def* new_def = def(pred_end->args[i]);
    pred_end->args[i] = 0;
    copyUses(&succ_start->params[i], new_def);
  }

  if (pred_end->catch_blocks != NULL) {
    // Merge these into succ
    BlockEndInstr* succ_end = blockEnd(succ_start);
    if (succ_end->catch_blocks == NULL) {
      // Successors didn't have any, so just transfer them
      succ_end->catch_blocks = pred_end->catch_blocks;
      for (SeqRange<ExceptionEdge*> r(*pred_end->catch_blocks); !r.empty(); r.popFront()) {
        if (enable_verbose) {
          lattice.console() << "adjusting exception edge from i" << r.front()->from->id << " -> i" << r.front()->to->id << "\n";
        }
        r.front()->from = succ_end;
        if (enable_verbose) {
          lattice.console() << "adjusting exception edge to i" << r.front()->from->id << " -> i" << r.front()->to->id << "\n";
        }
      }
    } else {
      // Merge the lists
      for (CatchBlockRange succ_catch(succ_end); !succ_catch.empty();) {
        CatchBlockInstr* catch_block = succ_catch.popFront();
        for (CatchBlockRange pred_catch(pred_end); !pred_catch.empty();) {
          if (catch_block == pred_catch.popFront()) {
            catch_block = NULL;
            break;
          }
        }
        if (catch_block != NULL) {
          ExceptionEdge* edge = new (alloc_) ExceptionEdge(succ_start, catch_block);
          pred_end->catch_blocks->add(edge);

          if (enable_verbose)
            lattice.console() << "adding exception edge i" << succ_start->id << " -> i" << catch_block->id << "\n";
          
          ExceptionEdge* N = catch_block->catch_preds;
          ExceptionEdge* P = N->prev_exception;
          edge->next_exception = N;
          edge->prev_exception = P;
          N->prev_exception = edge;
          P->next_exception = edge;
        }
      }
      // Disconnect this block
      for (SeqRange<ExceptionEdge*> r(*pred_end->catch_blocks); !r.empty();) {
        ExceptionEdge* edge = r.popFront();
        ExceptionEdge* N = edge->next_exception;
        ExceptionEdge* P = edge->prev_exception;
        if (edge->to->catch_preds == edge) {
          edge->to->catch_preds = N;
        }
        N->prev_exception = P;
        P->next_exception = N;
      }
    }
  }

  InstrRange block(succ_start);
  block.unlinkFront(); // remove succ_start from its block
  replaceInstr(pred_end, block);
}

/// TODO
///
InstrRange InstrGraph::returnBlock() const {
  StopInstr* r = returnStmt();
  return InstrRange(blockStart(r), r);
}

void InstrGraph::swapArms(IfInstr* cond) {
  ArmInstr* arm0 = cond->arms[0];
  ArmInstr* arm1 = cond->arms[1];
  arm0->arm_pos = 1;
  arm1->arm_pos = 0;
  cond->arms[0] = arm1;
  cond->arms[1] = arm0;
}

/// InstrGraphBuilder constructor.
///
InstrGraphBuilder::InstrGraphBuilder(InstrGraph* ir, InstrFactory* factory,
                                     Instr* pos)
: ir_(ir)
, factory_(*factory)
, pos_(pos)
, constants_(ir->alloc()) {
}

/// Add instruction to graph in progress.
/// gives instruction an id, checks input types, and
/// computes output types. returns its argument.
///
Instr* InstrGraphBuilder::addInstr(Instr* instr) {
  assert(!instr->isLinked());
  if (isBlockStart(instr))
    return pos_ = ir_->addBlock((BlockStartInstr*)instr);
  return ir_->addInstrBefore(pos_, instr);
}

PrintWriter& InstrGraphBuilder::console() const {
  return factory_.lattice().console();
}

void InstrGraphBuilder::setPos(Instr* instr) {
  assert(isBlockStart(instr));
  pos_ = instr;
}

/// Create a const-generating instr of the given type if one does not already
/// exist.  Since constants are only created once, link them immediately
/// after ir->begin to ensure the constant dominates all uses.
/// In the presence of exception edges this enforced sharing results
/// problems in the register allocator, so treat them as normal
/// instructions and let value numbering share them where it's possible locally.
Def* InstrGraphBuilder::addConst(const Type* t) {
  assert(isConst(t));
#if 0
  TypeKey k(t);
  Def* c = constants_.get(k);
  if (!c) {
    ConstantExpr* instr = factory_.newConstantExpr(HR_const, t);
    constants_.put(k, c = instr->value());
    ir_->addInstrAfter(ir_->begin, instr);
  }
  return c;
#endif
  ConstantExpr* instr = factory_.newConstantExpr(HR_const, t);
  addInstr(instr);
  return instr->value();
}

/// Compute the types of i's defs.
///
void InstrGraphBuilder::computeType(Instr* i) {
  TypeAnalyzer analyzer(ir_);
  analyzer.computeTypes(i);
}

/// Reassign uses from Def "from" to "to".  The easy way to do this
/// is fill(UseRange(old_def), this), but we can do better by explicitly
/// setting each Use.def_ = to, then joining the two use lists in a single step.
/// fixme: should this be called moveUses()?
///
/**
 *
 * @param before_def is a Def that originally (before the call) feeds some use.
 * @param after def will be made the definition of each Use of before_def,
 * so as to "short-circuit" before_def.
 * (A motivating use case is when we inline a method, when old_def is the
 * defn of a formal parm and new_def is the definition of an actual argument.)
 * copyUses moves each use of old_def to be a use of new_def instead)
 */
void copyUses(Def* old_def, Def* new_def)
{
  Use* front2 = old_def->uses_;
  if (!front2)
    return; //no uses, so nothing to do.

  //move each use of old_def to new_def.
  for (UseRange u(*old_def); !u.empty(); u.popFront())
    u.front().def_ = new_def;

  //zap old_def.uses so it has no more uses.
  old_def->uses_ = 0;

  //update new_def use list
  Use* front1 = new_def->uses_;
  if (front1) {
    //link in the new uses moved over from old_def
    // Concatenate [front1:back1] and [front2:back2].
    Use* back1 = front1->prev_;
    Use* back2 = front2->prev_;
    back1->next_ = front2;
    front2->prev_ = back1;
    back2->next_ = front1;
    front1->prev_ = back2;
  } else {
    // new_def had no uses before, so Just take the whole list.
    new_def->uses_ = front2;
  }
}

/// Redirect uses of old_instr's defs to new_instr's defs.
///
void copyAllUses(Instr* old_instr, Instr* new_instr) {
  assert(numDefs(new_instr) == numDefs(old_instr));
  // For each value defined by old_instr, point every use to the
  // corresponding def in new_instr.
  ArrayRange<Def> last_defs = defRange(new_instr);
  ArrayRange<Def> callsite_defs = defRange(old_instr);
  for (; !last_defs.empty(); last_defs.popFront(), callsite_defs.popFront())
    copyUses(&callsite_defs.front(), &last_defs.front());
}

/// Return the total number of Defs in instr.
/// TODO remove special cases once Info knows IR5
///
int numDefs(const Instr* instr) {
  switch (kind(instr)) {
    case HR_start:
    case HR_template:
    case HR_catchblock:
    case HR_label:
      return ((BlockHeaderInstr*)instr)->paramc;
    case HR_arm:
      return numArgs(((ArmInstr*)instr)->owner);
    default:
      assert(instr->info->num_defs >= 0);
      return instr->info->num_defs;
  }
}

/// Return a pointer to a contiguous array of all instr's Defs.
/// TODO remove special cases once Info knows IR5
///
Def* getDefs(const Instr* instr) {
  switch (kind(instr)) {
    case HR_start:
    case HR_template:
    case HR_catchblock:
    case HR_label:
    case HR_arm:
      return ((BlockStartInstr*)instr)->params;
    default:
      assert(instr->info->defs_off >= 0);
      return (Def*) ((char*) instr + instr->info->defs_off);
  }
}

/// Return the total number of Uses in instr.
/// TODO remove special cases once Info knows IR5.  This switch, and others
/// like it in numArgs(), etc, have shown to be hot.  We can avoid them
/// by creating distinct InstrInfo instances for different arity values of
/// switch, return, throw, goto, label, etc.
///
int numUses(const Instr* instr) {
  switch (kind(instr)) {
    case HR_if:
    case HR_switch:
      return ((CondInstr*)instr)->argc + 1;
    case HR_return:
    case HR_throw:
      return cast<StopInstr>(instr)->argc;
    case HR_goto:
      return numDefs(cast<GotoInstr>(instr)->target);
    default:
      assert(instr->info->num_uses >= 0);
      return instr->info->num_uses;
  }
}

/// Return the number of argument Uses in a BlockEndInstr.
/// TODO remove once Info knows IR5
///
int numArgs(const BlockEndInstr* instr) {
  switch (kind(instr)) {
    case HR_if:
    case HR_switch:
    case HR_return:
    case HR_throw:
      return ((BlockFooterInstr*)instr)->argc;
    case HR_goto:
      return numDefs(cast<GotoInstr>(instr)->target);
    default:
      assert(false && "invalid opcode for BlockEndInstr");
      return 0;
  }
}

/// Return a pointer to a contiguous array of all instr's Uses.
/// TODO remove special cases once Info knows IR5
///
Use* getUses(const Instr* instr) {
  switch (kind(instr)) {
    case HR_if:
    case HR_switch:
      return ((CondInstr*) instr)->uses;
    case HR_return:
    case HR_throw:
    case HR_goto:
      return ((BlockEndInstr*)instr)->args;
    default:
      assert(instr->info->uses_off >= 0);
      return (Use*) ((char*) instr + instr->info->uses_off);
  }
}

/// Return an allocated array of the types of uses of an instruction.
///
const Type** getUseTypes(Allocator& alloc, Instr* instr) {
  int argc = numUses(instr);
  Use* args = getUses(instr);
  const Type** types = new (alloc) const Type*[argc];
  for (int i = 0; i < argc; ++i)
    types[i] = type(args[i]);
  return types;
}

/// Return a pointer to a contiguous array of a BlockEndInstr's arg Uses.
/// TODO remove special cases once Info knows IR5
///
Use* getArgs(const BlockEndInstr* instr) {
  switch (kind(instr)) {
    case HR_if:
    case HR_switch:
    case HR_return:
    case HR_throw:
    case HR_goto:
      return instr->args;
    default:
      assert(false && "invalid opcode for BlockEndInstr");
      return 0;
  }
}

/// return a Range over the arms of a CondInstr
///
ArrayRange<ArmInstr*> armRange(CondInstr* instr) {
  return ArrayRange<ArmInstr*>(instr->arms, instr->armc);
}

/// true if instruction starts a block
///
bool isBlockStart(const Instr* instr) {
  switch (kind(instr)) {
    case HR_start:
    case HR_template:
    case HR_catchblock:
    case HR_label:
    case HR_arm:
      return true;
    default:
      return false;
  }
}

/// true if instruction ends a block
///
bool isBlockEnd(const Instr* instr) {
  switch (kind(instr)) {
    case HR_if:
    case HR_switch:
    case HR_return:
    case HR_throw:
    case HR_goto:
      return true;
    default:
      // templates that end with block ends (besides HR_return)
      // are block ends themselves
      if (hasSubgraph(instr)) {
        Instr* last = subgraph(instr)->end;
        if (last && kind(last) != HR_return && isBlockEnd(last))
          return true;
      }
      return false;
  }
}

bool isCond(const Instr* instr) {
  return kind(instr) == HR_if || kind(instr) == HR_switch;
}

/// True if an instruction has root Defs, i.e. Defs whose types
/// are axiomatic. Defs introduced by ConstantExprs and 
/// predecessor-free block starts have this property.
///
bool hasRootDefs(const Instr* instr) {
  InstrShape s = shape(instr);
  return s == CONSTANTEXPR_SHAPE || s == STARTINSTR_SHAPE || s == CATCHBLOCKINSTR_SHAPE;
}

/// true if end/start are a pair of related delimters, 
/// i.e. if end is predecessor and start is successor.
/// 
bool isDelimPair(const Instr* end, const Instr* start) {
  return ((kind(start) == HR_arm && ((ArmInstr*)start)->owner == end) ||
          (kind(end) == HR_goto && ((GotoInstr*)end)->target == start));
}

ArmInstr* getConstArm(CondInstr* cond, const Type* t) {
  InstrKind k = kind(cond);
  if (k == HR_if) {
    return cast<IfInstr>(cond)->arm(boolVal(t));
  } else {
    assert(k == HR_switch);
    return cast<SwitchInstr>(cond)->arm(intVal(t));
  }
}

/// if CondInstr's selector is constant, return the
/// selected arm, otherwise null.
/// 
ArmInstr* getConstArm(CondInstr* cond) {
  const Type* t = type(cond->selector());
  return isConst(t) ? getConstArm(cond, t) : 0;
}

/// return the position of an Arm within its owner's array
///
int pos(ArmInstr* arm) {
  return arm->arm_pos;
}

/// reset the types of an instr's defs to UN
///
void resetTypes(Instr* instr) {
  for (ArrayRange<Def> r = defRange(instr); !r.empty(); r.popFront())
    setType(&r.front(), BOT);
}

EachBlock::EachBlock(InstrGraph* ir, bool reverse) :
    ir(ir), visited(scratch, ir->block_count()), blocks(scratch), iter(0)
{
  dfs(ir->begin, reverse);
  this->iter = SeqRange<BlockStartInstr*>(blocks.get()); 
}

/// Perform a depth-first search starting at the given 
/// block start delimiter, if it hasn't already been 
/// visited. Called from the constructor, this builds 
/// a postorder or reverse-postorder sequence (as 
/// dictated by second param) of block start delimiters
/// in our blocks member variable.
///
void EachBlock::dfs(BlockStartInstr* block, bool reverse) {
  if (visited.get(block->blockid))
    return;
  visited.set(block->blockid);
  if (ir->hasBlockEnd(block)) {
    BlockEndInstr* end = ir->blockEnd(block);
    InstrKind k = kind(end);
    // recurse into successors
    if (k == HR_goto) {
      dfs(cast<GotoInstr>(end)->target, reverse);
    } else if (k == HR_if || k == HR_switch) {
      for (ArrayRange<ArmInstr*> r = armRange((CondInstr*)end); !r.empty();)
        dfs(r.popFront(), reverse);
    } else if (k == HR_return || k == HR_throw) {
      // no successors
    } else {
        assert(false && "unsupported block-end opcode");
    }
    if (end->catch_blocks != NULL) {
      for (CatchBlockRange r(end); !r.empty();)
        dfs(r.popFront(), reverse);
    }
    // add to block list
    if (reverse)
      blocks.insert(block);
    else
      blocks.add(block);
  }
}

void pruneGraph(InstrGraph* ir) {
  Allocator scratch;
  BitSet mark(scratch, ir->size());
  for (AllInstrRange i(ir); !i.empty(); i.popFront())
    mark.set(i.front()->id);
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    for (AllUsesRange u(instr); !u.empty(); u.popFront()) {
      Use& use = u.front();
      if (!mark.get(user(use)->id)) {
        if (enable_verbose)
          printf("prune: use i%d %s ->i%d\n", user(use)->id, name(user(use)),
                 instr->id);
        use = 0;
      }
    }
    if (kind(instr) == HR_label) {
      for (PredRange p(cast<LabelInstr>(instr)); !p.empty();) {
        GotoInstr* go = p.popFront();
        if (!mark.get(go->id)) {
          if (enable_verbose)
            printf("prune: goto i%d ->i%d\n", go->id, instr->id);
          removeGoto(go);
        }
      }
    }
    if (kind(instr) == HR_catchblock) {
      CatchBlockInstr* cblock = cast<CatchBlockInstr>(instr);
      ExceptionEdge* head = cblock->catch_preds;
        
      for (ExceptionEdge* edge = head; edge != NULL; ) {
        ExceptionEdge* next = edge == cblock->catch_preds->prev_exception ? NULL : edge->next_exception;
        if (!mark.get(edge->from->id)) {
          if (enable_verbose)
            printf("prune: eliminated exception edge i%d -> i%d\n", edge->from->id, cblock->id);
          if (edge->next_exception == edge) {
            cblock->catch_preds = NULL;
          } else {
            ExceptionEdge* N = edge->next_exception;
            ExceptionEdge* P = edge->prev_exception;
            if (cblock->catch_preds == edge) {
              head = cblock->catch_preds = N;
            }
            N->prev_exception = P;
            P->next_exception = N;
          }
        }
        edge = next;
      }
    }
  }
  if (ir->end && !mark.get(ir->end->id))
    ir->end = 0;
  if (ir->exit && !mark.get(ir->exit->id))
    ir->exit = 0;
  assert(checkPruned(ir));
}

/** true if x is a constant int and equal to c */
bool match(Def* x, int c) {
  const Type* t = type(x);
  return isConst(t) && isInt(t) && intVal(t) == c;
}

/** true if ix is unary opcode k.  If so return the input def */
bool match(Instr* ix, InstrKind k, Def** val) {
  if (kind(ix) == k)
    return (*val = def(cast<UnaryExpr>(ix)->value_in())), true;
  return false;
}

bool match(Def* x, InstrKind k, Def** val) {
  Instr* ix = definer(x);
  if (kind(ix) == k)
    return (*val = def(cast<UnaryExpr>(ix)->value_in())), true;
  return false;
}

bool match(const Use& x, InstrKind k, Def** val) {
  Instr* ix = definer(x);
  if (kind(ix) == k)
    return (*val = def(cast<UnaryExpr>(ix)->value_in())), true;
  return false;
}

/** true if type(d) is a subtype of t */
bool match(Def* d, const Type* t) {
  return subtypeof(type(d), t);
}

} // namespace halfmoon
#endif // VMCFG_HALFMOON
