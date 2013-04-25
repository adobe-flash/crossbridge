/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON
#include "profiler/profiler-main.h"

namespace halfmoon {
using avmplus::CodeMgr;
using avmplus::DomainMgr;
using avmplus::ScopeTypeChain;

using profiler::MethodProfile;

int enable_builtins = 1;
int enable_dvn = 1;
int enable_deopt = 0;
int enable_gml = 0;
int enable_inline = 0;
int enable_mode = 0;			// disabled by default, use -Dhalfmoon or env var
int enable_optional = 1;
int enable_peephole = 0;
int enable_printir = 0;
int enable_profiler = 0;
int enable_selftest = 0;
int enable_trace = 0;
int enable_try = 1;
int enable_framestate = 0;
int enable_typecheck = 0;
int enable_verbose = 0;
int enable_vmstate = 0;
int enable_welcome = 0;
ScheduleKind enable_schedule = kScheduleMiddle;

void init() {
  static bool first = true;
  if (!first)
    return;
  first = false;

  enable_builtins = parseEnv("BUILTINS", enable_builtins);
  enable_dvn = parseEnv("DVN", enable_dvn);
  enable_deopt = parseEnv("DEOPT", enable_deopt);
  enable_gml = parseEnv("GML", enable_gml);
  enable_inline = parseEnv("INLINE", enable_inline);
  enable_mode = parseEnv("MODE", enable_mode);
  enable_optional = parseEnv("OPTIONAL", enable_optional);
  enable_peephole = parseEnv("PEEPHOLE", enable_peephole);
  enable_printir = parseEnv("PRINTIR", enable_printir);
  enable_profiler = parseEnv("PROFILER", enable_profiler);
  enable_selftest = parseEnv("SELFTEST", enable_selftest);
  enable_trace = parseEnv("TRACE", enable_trace);
  enable_try = parseEnv("TRY", enable_try);
  enable_framestate = parseEnv("FRAMESTATE", enable_framestate);
  enable_typecheck = parseEnv("TYPECHECK", enable_typecheck);
  enable_verbose = parseEnv("VERBOSE", enable_verbose);
  enable_vmstate = parseEnv("VMSTATE", enable_vmstate);
  enable_welcome = parseEnv("WELCOME", enable_welcome);
  enable_schedule = (ScheduleKind)parseEnv("SCHEDULE", enable_schedule);

  debugInit();

  if (enable_welcome) {
    printf("Halfmoon environment:\n");
    printf("  BUILTINS  %d  Also look at the builtins\n", enable_builtins);
    printf("  DEOPT     %d  Enable deoptimization support (incomplete)\n", enable_deopt);
    printf("  DVN       %d  Dominator Value Numbering\n", enable_dvn);
    printf("  GML       %d  Generate GML graph output for yEd\n", enable_gml);
    printf("  INLINE    %d  Enable inlining\n", enable_inline);
    printf("  MODE      %d  Halfmoon mode. 0=none, 1=analyze, 2=interpret,"
           " 3=lir-stubs, 4=lir\n", enable_mode);
    printf("  OPTIONAL  %d  Enable optional-argument support\n", enable_optional);
    printf("  PEEPHOLE  %d  Enable ABC peephole optimizer\n", enable_peephole);
    printf("  PRINTIR   %d  Print final IR\n", enable_printir);
    printf("  PROFILER  %d  Enable runtime profiling\n", enable_profiler);
    printf("  SCHEDULE  %d  Scheduler 0=none, 1=early, 2=late, 3=middle\n", enable_schedule);
    printf("  SELFTEST  %d  Enable self-test\n", enable_selftest);
    printf("  TRACE     %d  Print execution trace\n", enable_trace);
    printf("  TRY       %d  Consider methods with try/catch.\n", enable_try);
    printf("  TYPECHECK %d  Enable type-check verbosity\n", enable_typecheck);
    printf("  VERBOSE   %d  Generate verbose output\n", enable_verbose);
    printf("  VMSTATE   %d  Add Safepoint instructions\n", enable_vmstate);
    printf("  WELCOME   %d  Generate this option list\n", enable_welcome);
    debugWelcome();
  }

  if (enable_selftest) {
#ifdef DEBUG
    // TODO probably not the right way to do this,
    // need some kind of junit like thing
    printf("self-test...");
    testRangeConstraints();
    printf("passed\n");
#else
    printf("self-test enabled only in debug builds\n");
#endif
  }
}

/** iterate over the abc instructions in a method, in linear order */
class AbcRange {
public:
  AbcRange(MethodInfo* method);

  bool empty() const {
    return pc >= end;
  }

  AbcOpcode front() const {
    assert(!empty());
    return (AbcOpcode) *pc;
  }

  void popFront() {
    assert(!empty());
    read();
  }

private:
  void read();

private:
  const uint8_t* pc;
  const uint8_t* next;
  const uint8_t* end;
};

AbcRange::AbcRange(MethodInfo* method) {
  pc = method->abc_body_pos();
  AvmCore::skipU32(pc, 4);
  int len = AvmCore::readU32(pc);
  end = pc + len;
  next = pc;
  read();
}

void AbcRange::read() {
  pc = next;
  uint32_t imm30, imm30b;
  int32_t imm24, imm8;
  AvmCore::readOperands(next, imm30, imm24, imm30b, imm8);
  if (*pc == OP_lookupswitch)
    next += 3 * (imm30b + 1);
}

bool canCompile(MethodInfo* m) {
  init();

  if (enable_mode == kModeNone)
    return false;

#ifdef VMCFG_DEBUGGER
  if (m->pool()->core->debugger())
    return false; // Cannot compile debuggable functions.
#endif
  if (m->isNative()) {
    return false;
  }
  if (!enable_builtins && m->pool()->isBuiltin)
    return false; // Ignore builtin code.
  if (!enable_try && m->hasExceptions())
    return false; // Ignore methods with exceptions.
  if (m->method_id() < 0) {
    return false; // Ignore vm-created initializer methods.
  }
  if (m->needArguments()) {
    return false; // Only rest args supported yet.
  }
  if (!enable_optional && m->hasOptional()) {
    return false;
  }

  if (enable_profiler) {
    MethodProfile* profile = JitManager::getProfile(m);
    if (!profile || (!profile->hasBailedOut() && !profile->hasGatheredAllData()))
      return false;
  }

  return true;
}

bool isProfilerEnabled() {
  return enable_profiler != 0;
}

/// Recursively mark instr and all the CondInstr's in its reverse dominance
/// frontier, which are the instructions it is control dependent on.
void markEffectRdf(BitSet* marks, DominatorTree* rdoms,
                   Instr* instr, BlockStartInstr* block) {
  if (marks->get(instr->id))
    return;
  marks->set(instr->id);
  // recursively visit each block in RDF(block)
  for (SeqRange<BlockStartInstr*> r(rdoms->df(block)); !r.empty();) {
    BlockStartInstr* rdf_block = r.popFront();
    markEffectRdf(marks, rdoms, InstrGraph::blockEnd(rdf_block), rdf_block);
  }
}

/// Mark each instruction that produces a linear-type value, as well
/// as any branches that control whether or not the effectful instruction
/// will run.  Those branches are the control dependencies, or equivalently,
/// they are the branches in the instruction's reverse dominance frontier.
void markEffects(InstrGraph* ir, BitSet* marks) {
  Allocator scratch;
  DominatorTree* rdoms = reverseDoms(scratch, ir);
  for (EachBlock b(ir); !b.empty();) {
    BlockStartInstr* block = b.popFront();
    /// search for effectful instructions in this block and stop early
    /// when/if this block gets marked.
    for (InstrRange i(block); !i.empty();) {
      Instr* instr = i.popFront();
      if (isBlockStart(instr) || isBlockEnd(instr))
        continue; // skip block delimiters
      for (ArrayRange<Def> d = defRange(instr); !d.empty();) {
        if (isLinear(type(d.popFront()))) {
          markEffectRdf(marks, rdoms, instr, block);
          break;
        }
      }
    }
  }

  if (enable_verbose) {
    for (EachBlock b(ir); !b.empty(); b.popFront()) {
      BlockStartInstr* block = b.front();
      BlockEndInstr* end = ir->blockEnd(block);
      if (kind(end) == HR_if && !marks->get(end->id))
        if (enable_verbose)
          printf("dead: useless i%d %s\n", end->id, name(end));
    }
  }
}

/// if a conditional instruction splits EFFECT, but no effectful
/// instructions are control-dependent on it, then unsplit effect
/// because its an unnecessary data dependency.
Def* unsplit(ArmInstr* arm, Def* d, BitSet* marks) {
  CondInstr* cond = arm->owner;
  if (isLinear(type(d)) && !marks->get(cond->id)) {
    // nothing is control-dependent on cond, so we can unsplit d.
    return def(cond->arg(pos(d)));
  }
  return d;
}

/// Replace references to instructions with references to the instruction's
/// identity instr (if different).
void computeIdentities(InstrGraph* ir) {
  assert(checkPruned(ir) && checkSSA(ir));
  Allocator scratch;
  BitSet marks(scratch, ir->size());
  markEffects(ir, &marks);

  InstrFactory factory(ir);
  bool changed;
  do {
    changed = false;
    for (AllInstrRange i(ir); !i.empty();) {
      Instr* instr = i.popFront();
      InstrKind k = kind(instr);
      for (ArrayRange<Def> d = defRange(instr); !d.empty();) {
        Def* d1 = &d.popFront();
        if (!d1->isUsed())
          continue;
        Def* d2 = peephole(d1, ir, &factory);
        if (k == HR_arm && d1 == d2)
          d2 = unsplit(cast<ArmInstr>(instr), d1, &marks);
        if (d1 != d2) {
          assert(subtypeof(type(d2), type(d1)));
          copyUses(d1, d2);
          changed = true;
        }
      }
    }
  } while (changed);
  assert(checkPruned(ir) && checkSSA(ir));
}

/// TODO splain
///
Def* peephole(Def* def, InstrGraph* ir, InstrFactory* factory) {
  assert(definerId(def) != -1 && "Instruction is not initialized");
  Instr* instr = definer(def);

  const Type* t = type(def);
  if (isConst(t) && kind(instr) != HR_const) {
    // fixme: need value numbering here to reuse consts
    ConstantExpr* const_instr = factory->newConstantExpr(HR_const, t);
    ir->addInstrAfter(instr, const_instr);
    if (enable_verbose) {
      printf("replaced i%d d%d with i%d\n",
             definerId(def), pos(def), const_instr->id);
    }
    return const_instr->value();
  }

  return IdentityAnalyzer(def, &ir->lattice).identity(instr);
}

/// Redirect uses of new_start's defs to outer_stmt's inputs.
/// new_start's defs are the inlined code's formal parameters.
///
void connectInnerUsesToOuterUses(Instr* new_start, Instr* outer_stmt) {
  assert(numDefs(new_start) == numUses(outer_stmt));
  ArrayRange<Def> start_defs = defRange(new_start);
  ArrayRange<Use> outer_inputs = useRange(outer_stmt);
  for (; !outer_inputs.empty(); start_defs.popFront(), outer_inputs.popFront())
    copyUses(&start_defs.front(), def(outer_inputs.front()));
}

/// Redirect uses of callsite defs to the defs that are inputs to ret_stmt.
///
void connectOuterUsesToInnerUses(Instr* ret_stmt, Instr* callsite) {
  assert(numUses(ret_stmt) == numDefs(callsite));
  // For each value defined by the callsite, change every use to refer to the
  // correspnding value **used** by the return statement.  The return statement
  // ends up being a straggler.
  ArrayRange<Use> ret_inputs = useRange(ret_stmt);
  ArrayRange<Def> callsite_defs = defRange(callsite);
  while (!callsite_defs.empty())
    copyUses(&callsite_defs.popFront(), def(ret_inputs.popFront()));
}

///
///
void replaceStop(InstrGraph* ir, InstrFactory* factory, StopInstr* stop,
                 LabelInstr* target) {
  GotoInstr* go = factory->newGotoStmt(target);
  copyRange(useRange(stop), useRange(go));
  ir->replaceInstr(stop, go);
}

///
///
StopInstr* joinStops(InstrGraph* ir, InstrFactory* factory, StopInstr* stop1,
                     StopInstr* stop2) {
  // If either stop is null, return the other one.
  if (!stop1)
    return stop2;
  if (!stop2)
    return stop1;

  assert(kind(stop1) == kind(stop2));
  LabelInstr* label = factory->newLabelInstr(numUses(stop1));
  ir->addBlock(label);

  replaceStop(ir, factory, stop1, label);
  replaceStop(ir, factory, stop2, label);
  copyRange(defRange(label), useRange(stop1));
  ir->linkAfter(label, stop1);
  return stop1;
}

/// Do inline expansion of a single statement node which has effect and data.
///
/// The out-arity of the inner start node (HR_template) must match the
/// in-arity of outer_stmt, and the in-arity of the return statement in the
/// inlined code must match the out-arity of outer_stmt.  Arguments and
/// results are rewired by position.
///
/// Inline expansion is done in five steps:
/// 0. Typecheck inner_ir (outer_stmt's subgraph) using outer_stmt's
///    input types.
/// 1. Set up a copier to clone nodes as they are requested.
/// 2. Connect inner_ir inputs to outer_stmt inputs.
/// 3. Connect inner_ir outputs to outer_stmt outputs.
/// 4. replace outer_stmt with inner_ir instructions.
/// \param outer_ir the instruction graph instruction will be added to
/// \param inner_ir
/// \param outer_stmt the statement in the outer_ir that is being inlined.
///
void expandStmt(Context*, InstrGraph* outer_ir, InstrGraph* inner_ir,
                Instr* outer_stmt) {
  PrintWriter &pw = outer_ir->lattice.console();
  if (enable_verbose) {
    pw << "expandStmt ";
    printInstr(pw, outer_stmt);
  }

  // 0. evaluate the types on the inner_ir according to the input types
  //    at the call site (outer_stmt).
  propagateTypes(outer_stmt);

  // 1. Set up a graph copier.  Each copy is done deeply.
  Copier copier(inner_ir, outer_ir);
  assert(kind(inner_ir->begin) == HR_template);
  InstrRange start_block(copier.copy(inner_ir->begin));
  InstrRange return_block = outer_ir->blockRange(copier.copy(
      inner_ir->returnStmt()));

  if (enable_verbose)
    printTerseInstrList(start_block, inner_ir,
        "begin: block cloned from begin of template");

  // 2. Connect inputs to inputs.
  // Connect uses of inner_ir's formal parameters (defs in its start node)
  // to the defs that are inputs to outer_stmt.
  connectInnerUsesToOuterUses(start_block.front(), outer_stmt);

  // 3. Connect outputs to outputs.  Inner_ir ends with a return instruction,
  // bug we dont want the return; uses of outer_stmt's defs are redirected to
  // these defs.
  Instr* return_instr = return_block.back();
  InstrFactory factory(outer_ir);
  InstrGraphBuilder builder(outer_ir, &factory, return_instr);
  connectOuterUsesToInnerUses(return_instr, outer_stmt);

  // 4. Replace outer_stmt in outer_ir with start_block and return_block.
  outer_ir->inlineBlocks(outer_stmt, start_block, return_block);
}

/**
 * sketch of scalarizing the newactivation object:
 * 1. early bind findprop, getprop, setprop
 * 2. if all uses of newactivation are just getslot/setslot, then newactivation
 *    did not escape.
 * 3. we can compute reaching defs, and insert refs and phis.
 *
 * can we do this sparsely?  for example, the newactivation instr has the list
 * of all the getslot and setslot instructions, and we dont care about any
 * others.  what else do we need?  (dominance?)
 */

/// Expand fat templates and perform early binding.
///
void expandTemplates(Context* context, InstrGraph* ir) {
  assert(checkPruned(ir) && checkSSA(ir));
  Specializer specializer(ir);
  for (AllInstrRange i(ir); !i.empty();) {
    Instr* instr = i.popFront();
    specializer.specialize(instr);
    if (ir->isLinked(instr) && hasSubgraph(instr))
      expandStmt(context, ir, subgraph(instr), instr);
  }
  pruneGraph(ir);
  assert(checkPruned(ir) && checkSSA(ir));
}

void optimizePass(Context* cxt, InstrGraph* ir, const char* title) {
  assert(checkPruned(ir) && checkSSA(ir));
  propagateTypes(ir);
  if (enable_verbose) {
    printf("BEFORE pass %s\n", title);
    listCfg(cxt->out, ir);
  }
  computeIdentities(ir);
  removeDeadCode(cxt, ir);
  if (enable_dvn) {
    dominatorValueNumbering(cxt, ir);
    removeDeadCode(cxt, ir);
  }
  expandTemplates(cxt, ir);
  removeDeadCode(cxt, ir);
  if (enable_inline) {
    printGraph(cxt, ir, "before-inlining");
    bool ir_changed = inlineGraph(cxt, ir, title);
    printGraph(cxt, ir, "after-inlining");
    removeDeadCode(cxt, ir);
    if (ir_changed){
      printGraph(cxt, ir, "after-inlining-deadcode");
    }
  }
  printGraph(cxt, ir, title);
  assert(checkSSA(ir));
}

void optimize(Context* cxt, InstrGraph* ir) {
  optimizePass(cxt, ir, "1");
  optimizePass(cxt, ir, "2");
  optimizePass(cxt, ir, "3");
  optimizePass(cxt, ir, "4");

  fixModels(cxt, ir);
  printGraph(cxt, ir, "5-fixmodels");

  switch (enable_schedule) {
    default:
    case kScheduleNone:
      break;
    case kScheduleEarly:
      scheduleEarly(ir);
      printGraph(cxt, ir, "6-early");
      break;
    case kScheduleLate:
      scheduleLate(ir);
      printGraph(cxt, ir, "6-late");
      break;
    case kScheduleMiddle:
      scheduleMiddle(ir);
      printGraph(cxt, ir, "6-middle");
      break;
  }
}

/**
 * Do a depth-first traversal starting from the given block, inserting
 * finished blocks in list, so list ends up in reverse postorder.  Visit either
 * all edges or only normal edges, depending on visit_all param.
 */
static void dfs(AbcBlock* b, int& post_id, bool& has_reachable_exceptions, SeqBuilder<AbcBlock*> &list,
                bool visit_all) {
  b->post_id = -1;
  // visit ordinary successors.
  for (int i = 0, n = b->num_succ_blocks; i < n; ++i) {
    AbcBlock* succ = b->succ_blocks[i];
    if (!succ->post_id)
      dfs(succ, post_id, has_reachable_exceptions, list, visit_all);
    else if (succ->post_id == -1)
      succ->dfs_loop = true;
  }
  if (visit_all) {
    // visit catch blocks reachable from this block.
    for (int i = 0, n = b->max_catch_blocks; i < n; ++i) {
      AbcBlock* succ = b->catch_blocks[i];
      if (!succ)
        continue;
      if (succ->start_types == NULL)  // Unreachable catch block
        continue;
      has_reachable_exceptions = true;
      if (!succ->post_id)
        dfs(succ, post_id, has_reachable_exceptions, list, visit_all);
      else if (succ->post_id == -1)
        succ->dfs_loop = true;
    }
  }
  b->post_id = ++post_id;
  list.insert(b);
}

/// TODO comment
///matz: could add parm here to pass calling context when inlining.
InstrGraph* parseAbc(MethodInfo* method, Lattice* lattice, InfoManager* infos,
                     Allocator& ir_alloc, AbcGraph* abc, Toplevel *toplevel,
                     AbcEnv *abc_env, ProfiledInformation* profiled_info,
                     Context &cxt) {
  (void)abc_env; //TODO: change protocol of parseAbc
  // First, sort AbcBlocks in reverse postorder.
  Allocator scratch;
  SeqBuilder<AbcBlock*> list(scratch);
  int post_id = 0;
  bool has_reachable_exceptions = false;
  dfs(abc->entry(), post_id, has_reachable_exceptions, list, true);

  // Then visit the AbcBlocks and build an InstrGraph.
  InstrFactory factory(ir_alloc, lattice, infos);
  AbcBuilder builder(method, abc, &factory, toplevel, profiled_info, has_reachable_exceptions);
  InstrGraph* ir = builder.visitBlocks(list.get());


  if (enable_gml)
    printAbcCfg(method, list.get(), ir, "0-abc");

#ifdef DEBUG
  if (enable_verbose) {
    cxt.out << "original IR for " << cxt.method;
    listCfg(cxt.out, ir);
  }
#endif
  assert(checkSSA(ir));

  optimize(&cxt, ir);

#ifdef AVMPLUS_VERBOSE
  if (enable_printir || enable_verbose) {
    cxt.out << "final IR for " << cxt.method << " method_id=" <<
        cxt.method->method_id() << " ";
    listCfg(cxt.out, ir);
  }
#endif

  assert(checkTypes(ir, true));
  return ir;
}

} // end namespace halfmoon
#endif // VMCFG_HALFMOON
