/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using avmplus::Exception;
using avmplus::ExceptionFrame;
using avmplus::kCatchAction_Rethrow;
using avmplus::Verifier;

inline static void dbgDumpIr(InstrGraph *ir, const char *banner, Context *cxt);
inline static bool dbgInlineSkip(MethodInfo *method_info);
inline static bool dbgInlineOnly(MethodInfo *method_info);
inline static void dbgInlineCallsite(CallStmt2 *call, Context *cxt, InstrGraph *callee_ir);
inline static void dbgPrintGraph(Context *cxt,InstrGraph *ir, const char *title);
struct InlineWorklistItem;
inline static void dbgPrintInlineWorklist(Context *cxt, SeqBuilder<InlineWorklistItem>& work);
inline static void dbgDumpIr(Instr *call_site, InstrGraph* outer_ir, InstrGraph *callee_ir);

static bool inline_enable_verbose = false; //enable_verbose!=0;
static bool INLINE_ALWAYS_VERBOSE = false;

//make debug printout code less invasive
#ifdef AVMPLUS_VERBOSE
 #define INLINE_VERBOSE(CTX, ...) if (inline_enable_verbose){ CTX << __VA_ARGS__ << "\n";}
#else
 #define INLINE_VERBOSE(CTX,...) /*nada*/
 inline static void dbgDumpIr(InstrGraph *, const char *, Context *){}
 inline static bool dbgInlineSkip(MethodInfo *method_info){(void)method_info; return false;}
 inline static bool dbgInlineOnly(MethodInfo *){ return false;};
 inline static void dbgPrintGraph(Context *,InstrGraph *, const char *){}
 inline static void dbgPrintInlineWorklist(Context *, SeqBuilder<InlineWorklistItem>& ){}
 inline static void dbgInlineCallsite(CallStmt2 *, Context *, InstrGraph *){}
 inline static void dbgDumpIr(Instr *, InstrGraph* , InstrGraph *){};
#endif /*AVMPLUS_VERBOSE*/

/**
 *
 * @param call_site the call statement to be delved into
 * @return the MethodInfo of the target of the call.
 */
inline MethodInfo * targetOfCallSite(CallStmt2 *call_site) {
  //The first parm of the callmethod is the target of the call.
  //the value ought to be of type kTypeEnv.
  //it represents a MethodEnv that we can ask questions of (like finality)
  //
  assert(shape(call_site) == CALLSTMT2_SHAPE);
  const Use& target = call_site->param_in(); //target of call stmt is methodenv
  const Type *target_type = type(target);    //type is method_env
  assert(isEnv(target_type));                //better be a methodenv!
  MethodInfo* method_info = getMethod(target_type);
  return method_info;
}

/**
 * Are we able to inline the given call instruction?
 * Meaning do we have the machinery in place to compile
 * the method and does our inlining support this type
 * of call.
 * Note: this checks properties of the callee to see if the inling plumbing
 * can handle it, NOT whether it is correct to inline the callee at a specific callsite!
 */
bool ableToInline(Context *cxt, CallStmt2* call) {
  (void)cxt; //release build
  MethodInfo* method_info = targetOfCallSite(call); //getMethod(type(call->param_in()));

  if (!halfmoon::canCompile(method_info)){
    INLINE_VERBOSE(cxt->out, " not able to inline because not halfmoon.canCompile: " << method_info );
    return false;
  }

  //TOOD: teach downstream inlining logic to fill in default parameters from methodSig
  if (method_info->hasOptional()){
    if (method_info->getMethodSignature()->param_count() != call->arg_count()-2) {
        INLINE_VERBOSE(cxt->out, " not able to inline because non-specified optional arguments: " << method_info );
        return false;
      }
  }
  if (!method_info->declaringScope()){
    INLINE_VERBOSE(cxt->out, " not able to inline because not declaring scope: " << method_info );
    return false;
  }
  if (method_info->needRestOrArguments()){
    INLINE_VERBOSE(cxt->out, " not able to inline because needRestOrArguments: " << method_info );
    return false;
  }
  if (dbgInlineSkip(method_info)) {
    INLINE_VERBOSE(cxt->out, " skip inline because listed in DEBUG env var: " << method_info );
    return false;
  }
  return true;
}


/**
 * Is it semantically correct to statically inline the callee?
 * (statically means without a guard)
 * If the function itself is final, then can inline.
 * If the class defining the function is final, then can inline.
 *
 * @param cxt compilation context -- info like what method we're currently compiling.
 * @param call_site callsite to be considered
 * @return true if call at callsite can be statically inlined
 */
static bool isCorrectToStaticallyInline(Context *cxt, CallStmt2* call_site) {
  (void)cxt; //release build

  switch(kind(call_site)) {
  case HR_callmethod:
    break;
  case HR_callstatic:
    return false; // these are always true?
  case HR_callinterface:
    return false; //I guess these will these always be speculative(?)
  default:
      AvmAssert(false); //not a callsite
  }
  MethodInfo* method_info = targetOfCallSite(call_site);
  Traits *declaring_traits = method_info->declaringTraits(); //class method is in.
  if (declaring_traits->final) {
    INLINE_VERBOSE(cxt->out, "correctToInline returns true for: " << method_info << " because class is final" );
    return true;
  }
  //if method is declared final then even if receiver
  //is of a derived class the method cannot have been overriden,
  //so it's safe to inline it
  //
  if (method_info->isFinal()) {
    INLINE_VERBOSE(cxt->out, "correctToInline returns true for: " << method_info << " because method is final" );
    return true;
  }
  INLINE_VERBOSE(cxt->out, "correctToInline returns true false: " << method_info);
  return false;
}


/**
 *
 * @param cxt Context of compilation.. now including "calling context", which,
 * inthe case of inlining, points to the Context of the caller of the
 * method currently being scanned for inlinable callsites. The list of
 * Contest structs are sort of like a stack traceback, except for nested inlines.
 * (Probably a better way to think of them is as a path through the call graph
 * of an inline nest. If the callee appears on the path it's recursive)
 * @param call_site points to a call instruction being considered as an
 * inlining  candidate.
 * @return true if the callsite represents a recursion, either directly
 * (eg. factorial method calls itself) or indirectly, where the callsite
 * calls a method that.
 */
bool isRecursiveCallsite(Context *cxt, CallStmt2* call_site) {
  MethodInfo* method_info = targetOfCallSite(call_site);

  //if method is calling itself it's obviously recursive
  if (method_info == cxt->method) {
    INLINE_VERBOSE(cxt->out, "isRecursiveCallsite returns true for: " << method_info << " because callsite is recursive" );
    return true;
  }
  //if method is about to call a method somewhere on its call chain, it's recursive
  for (const Context *aContext = cxt->caller; aContext != NULL; aContext = aContext->caller) {
    if (method_info == aContext->method) {
      INLINE_VERBOSE(cxt->out, "isRecursiveCallsite returns true for: " << method_info << " because callsite is recursive" );
      return true; //gotcha. method
    }
  }
  return false; //got to end of chain without finding self, so not recursive
}


/**
 * Fire up the JIT and compile a method.
 * motivating use case was for inlining.
 *
 * @param cxt
 * @param m method to compile
 * @return InstrGraph of the freshly compiled method, or null if compilation
 * could not be done.
 */
InstrGraph* compileCallee(Context *cxt, CallStmt2* call_site) {
  assert(ableToInline(cxt, call_site));
  MethodInfo* method_info = targetOfCallSite(call_site);
  InstrGraph* ir = JitManager::init(method_info->pool())->ir(method_info);

  if (ir){
    INLINE_VERBOSE(cxt->out, "found  IR (compiled previously) for: " << method_info->getMethodName());
    return ir;
  }
  // Code copied from way up stack: BaseExecMgr::verifyJ2()
  MethodSignaturep method_signature = method_info->getMethodSignature();
  assert(halfmoon::canCompile(method_info));

  //create a new verifier pipeline that will compile the callee
  //JitWriter instance will carry Context instance from
  //this compilation to "child" compilation, where it will
  //appear as the "calling context". In this way the nested compilations
  //will see a chain of Context instances, sort of like a stacktrace,
  //as methods are inlined
  //
  JitWriter jit(method_info, cxt);
  CodeWriter* volatile vcoder = &jit; // Volatile for setjmp safety.

  Verifier verifier(method_info, method_signature, cxt->toplevel, cxt->abc_env);
  AvmCore *core = method_info->pool()->core;
  TRY(core, kCatchAction_Rethrow) {
    INLINE_VERBOSE(cxt->out, "inliner requests compilation of: " << method_info->getMethodName());
    verifier.verify(vcoder);
  } CATCH (Exception *exception) {
    verifier.~Verifier(); // Clean up verifier.
    vcoder->cleanup(); // Cleans up all coders.
    // todo: fixme: this doesn't come close to cleaning up all the in-flight HM state.
    core->throwException(exception);
  }
  END_CATCH
  END_TRY
  InstrGraph *callee_ir = jit.ir();
  return callee_ir;
}


/**
 * An InlineWorklistItem saves the details we will need to inline callee_ir
 * into callsite. Original thought was to link these into a call graph of
 * inlined methods
 */
struct InlineWorklistItem {
  explicit InlineWorklistItem(CallStmt2* call)
  : call_site(call)
  , callee_ir(0)
  {  }
  CallStmt2* call_site;   //the callsite
  InstrGraph* callee_ir;  //cache compile IR for the method
};


/// TODO comment
///
void propagateTypesInliner(InstrGraph* callee_ir, CallStmt2 *call_site) {
  assert(kind(callee_ir->begin) == HR_start);
  StartInstr* start_instr = cast<StartInstr>(callee_ir->begin);

  // Watch out: same impedance mismatch as in connectInputs
  int param_count = start_instr->paramc;
  // the two extras are effect and methodEnv
  assert(call_site->arg_count() == param_count - 2);

  Allocator scratch;
  const Type** types = new (scratch) const Type*[param_count];
  types[0] = type(call_site->effect_in()); //type(item.effect_in);
  types[1] = type(call_site->param_in());  //type(item.env_in);
  for (int i = 0, n = call_site->arg_count(); i < n; ++i){
    types[i+2] = type(call_site->arg(i));
  }
  propagateTypes(callee_ir, types);
}

/**
 * @param callee_start_instr first instruction in a function. Defines
 * formal parameters
 * @param call_site specific callsite to be replaced with inline. Uses actuals.
 * connectInputs hooks the defs of the actuals to the uses of the formal parameters of the callee.
 *
 * this is almost identical to what we do to expand a statment template EXCEPT that the callmethod
 * statement takes extra arguments (like the one for the methodname)
 *
 * BEFORE: defs of arguments used by call statement
 *
 *       D2:----+         //D2, D1 reconnected to parms of start..
 *       D1: +  |
 *           |  |               +----------------+
 *           |  |               | start (parms)  |
 *           v  v                    |  |
 *       callsite(args)              |  |
 *                                1 <+  |
 *                                2 <---+
 *
 * AFTER: definers of arguments connected to users of formal parameters.
 *
 *       D2:----+      //D2, D1 reconnected to uses of formal parameters
 *       D1: +  |
 *           |  |
 *           |  |
 *           |  +-----------------------+
 *           +-----------------------+  |
 *       callsite                    |  |
 *                                1 <+  |
 *                                2 <---+
 *
 */
void connectInputs(StartInstr* callee_start_instr, CallStmt2 *call_site) {
  // CallStmt2s have an extra use (the disp_id), so the call site arguments are
  // not congruent with start's defs:
  //
  // callmethod: (effect,      object, [N extra args])
  // start:      (effect,      object, [N extra params])
  // TODO handle rest
  assert(call_site->arg_count() == callee_start_instr->paramc - 2);

  // start instruction doesn't count effect amongst its "data parameters"
  // ie. data_param(0)  is not the effect
  // whereas for method calls use(0) is the effect..

  int num_data_actual_args = call_site->info->num_uses -1;
  int extra_formal_parameters = callee_start_instr->data_param_count() - num_data_actual_args;
  AvmAssert(extra_formal_parameters == 0); //TODO: make sure true for all callsites.

  // Special case the effect Def of the start instruction
  copyUses(callee_start_instr->effect_out(), def(call_site->effect_in()));

  // For each formal parameter of the callee function (once EFFECT has been
  // special cased), connect each use (in the callee function
  // body) to the def of the corresponding argument on the callsite.
  //
  for (int i = 0; i < num_data_actual_args; ++i){
    //TODO: add data_param() to callstmt family?  +1 depends on innards callstm
    const Use& an_arg = call_site->use(i+1);
    Def * def_of_actual_arg = def(an_arg);
    Def *corresponding_formal_parm = callee_start_instr->data_param(i+extra_formal_parameters);
    //TODO: how do i distinguish between "FinalDefaultClass[O]~" and FinalDefaultClass[A]~"
    assert(type(def_of_actual_arg)->isSubtypeOf(*type(corresponding_formal_parm)));
    if (!corresponding_formal_parm->isUsed()) {
      continue; //formal has no uses nothing to do
    }
    copyUses(corresponding_formal_parm,def_of_actual_arg);
  }
}


/**
 * Clone the graph, which means copy all the instructions into the "outer"
 * graph into which we are inlining.  No use/def connections
 * are made (so at this point callee code is there, but dead).
 *
 * @param callee_ir instruction graph to be inlined
 * @param outer_ir  instruction graph containing destination of inlining.
 * @param p_start OUT parameter. ignored on input, set to point to start
 * instruction in outer ir.
 * @param p_ret  OUT paramter. ignored on input, set to point to ret instruction
 * in outer ir.
 */
void cloneCallee(InstrGraph* callee_ir, InstrGraph* outer_ir,
                 StartInstr** p_start, StopInstr** p_return,
                 StopInstr** p_throw) {
  Copier copier(callee_ir, outer_ir);
  *p_start = copier.copy(cast<StartInstr>(callee_ir->begin));
  *p_return = copier.copy(callee_ir->returnStmt());
  *p_throw = copier.copy(callee_ir->throwStmt());
}

//
// Inline policy
//
//   The contract that an inliner must follow is to call shouldAttemptInline()
//   passing the containing method (m),  the candidate for inlining and the
//   number of potential candidates that might be inlined in the containing method.
//
//   If this method returns false, it is expected that attempts to inline the
//   candidate, should be abandoned.
//
//   If true is returned, shouldInline() is to be called (once the IR is
//   available) for final validation that the candidate is to be inlined.
//
//
bool shouldAttemptInline(Context* cxt,
                         MethodInfo* /*candidate*/,
                         uint32_t potentialInlineCount) {
  // if we are nesting too deep then blanket disable
  if (cxt->inline_depth > 2)
    return false;
  // if the # of inlines for the method is high then lets blanket disable
  if (potentialInlineCount > 2000)
    return false;
  return true;
}

/**
 *
 * <Pre>
 * there are several steps to inlining a method.
 * aa) find the methodInfo corresponding to the callee.
 * a) recompile the callee. (todo: build cache of previously compiled callees)
 * 0) clone the callee IR and locate the beginning and end of the callee function.
 * 0a if the callee has no flow (ie. its body is only one bb) replaceStmt does the plumbing.
 *    o/w locate the bb at the head of the callee (entry_list) and the bb at the tail (ret_list).
 * 1) split the calling basic block at the callsite
 * 2) graft entry_list into the "outer" ir before the callsite
 * 3) graft ret_list into the "outer" ir after the callsite
 * 4) reconnect the uses and defs such that
 * 4a) defs formerly feeding the callsite now feed uses in the callee.
 * 4b) uses in the caller formerly fed by the return result of the
 *     callsite are now fed by defs in the callee that formerly fed the
 *     return statement of the callee.
 *
 *             CALLER                                  CALLEE
 *                                       + prev_     +-----------+      <-----+
 *                                       |           |entry_list |            |
 *  +-prev_ +------------+ <---+         |           |   ...     |            |
 *  |       |            |     |         |           |           |            |
 *  |       |            |     |         +----->     +-----------+      next_ +
 *  |       |   P        |     |
 *  |       +- callsite -+     |                <<<arbitrary flow within callee >>>
 *  |       |   N        |     |
 *  |       |            |     |         +-prev_-    +--------  -+     <------+
 *  |       |            |     |         |           |  ret_list |            |
 *  +---->  +------------+next_+         |           |  ...      |            |
 *                                       |           |           |            |
 *                                       +----->     +-----------+     next_ -+
 * outputs: instruction lists have been sucked into the block and the instruction at pos has been deleted:
 *
 *  +-prev_ +------------+ <---+
 *  |       |            |     |
 *  |       |   P        |     |
 *  |       +------------+     |
 *  |       | entry_list |     |
 *  |       |   ...      |     |
 *  |       |            |.....|......branch at end of entry_list has succs in cloned callee
 *  +--->   +------------+next_+
 *
 *      cloned CFG branched to by entry_list
 *      and branches to ret_list..
 *
 *  +prev_  +------------+  <---+
 *  |       |  ret_list  | .... |...... block at head of ret_list has preds in cloned callee
 *  |       |  ...       |      |
 *  |       +------------+      |
 *  |       |   N        |      |
 *  |       |            |      |
 *  +---->  +------------+ next_+
 *
 * Connecting inputs to outputs
 *
 *   Dc stmt  (effect=)  // stmt defining effect value in (dD1) in of callsite
 *   ...
 *   D2 op    (val=)     // instr defining data value in (dD2) of callsite
 *   ...
 *
 *   C  callmethod   (dDeffect dD2val) -- (effect=+ val=*) -> iU1,iU2
 *   ...
 *   U1   stmt       (dCeffect) //stmt consuming effect_out of callsite
 *   ...
 *   U2   instr      (dCval)   //instruction consuming data value out of callsite
 *
 *   1) we must connect the defs of D1 and D2 to uses in the head op(in place of the callsite)
 *   2) we must connect defs feeding the return statement in the cloned callee graph with uses at U1, U2
 *
 *       D2:----+     //D2, D1 reconnected to parms of start.. (See reconnectInputs)
 *       D1: +  |
 *           |  |               +----------------+
 *           |  |               | start (parm1)  |
 *           v  v
 *       callsite          <<<arbitrary flow within callee >>>
 *           |  |               +----------------+
 *           |  |               |   ret_list     |
 *           |  |               |  ...           |
 *           |  |                 A: ------+   // def reconnected to uses at U1 (See reconnectOutputs)
 *       U1 <+  |                 C: --+   |   // def reconnected to use at U2
 *       U2 <---+                      v   v
 *                                ret (d)
 *
 * inliner starts with a graph for an entire function.
 * it begins with a "start" instruction, which has an effect def, a return
 * result, and zero or more parms.
 *
 * \param outer_ir is the "main" ir we want to inline into
 * \param callee_ir is the graph we just created by compiling the callee
 * \param callsite_stmt
 *
 * </pre>
 */
void inlineCallsite(InstrGraph* outer_ir, InstrFactory* factory, InstrGraph *callee_ir,
                    const InlineWorklistItem& item) {
  Instr* callsite = item.call_site;

  assert(kind(callee_ir->begin) == HR_start);
  assert(checkOneEnd(callee_ir, callee_ir->end, HR_return));
  assert(checkOneEnd(outer_ir, outer_ir->exit, HR_throw));

  dbgDumpIr(callsite, outer_ir, callee_ir);



  // 0. evaluate the types on the inner_ir according to the input types
  //    at the call site (outer_stmt).
  // what does this mean for inlining? freshly compiled graph needs types
  // propagated to it. (That's how we push the context of the caller down into
  // the callee)
  propagateTypesInliner(callee_ir, item.call_site);
  printTerseInstrList(callee_ir->begin, outer_ir, "begin block of callee");

  // 1. Copy the IR of the callee into outer_ir. find the start_instr and
  // return instruction.  After this, all the instructions we touch are
  // in outer_ir.
  Copier copier(callee_ir, outer_ir);

  //copy the callees start,return,throw instructions.. just floating between graphs (?)
  StartInstr* start_instr = copier.copy(cast<StartInstr>(callee_ir->begin));
  StopInstr* return_instr = copier.copy(callee_ir->returnStmt());
  StopInstr* throw_instr = copier.copy(callee_ir->throwStmt());

  // 2. Connect uses of inner_ir's formal parameters (defs in its start_instr)
  // to the defs that are inputs to callsite, then unlink start_instr.
  connectInputs(start_instr, item.call_site);
  InstrRange start_block(start_instr);

  // 3. Connect uses of return results to defs of values returned, then
  // unlink return_instr.
  InstrRange return_block = outer_ir->blockRange(return_instr);
  printTerseInstrList(return_block, outer_ir,
              "outer ret_block: block cloned from return block of callee ir");
  connectOuterUsesToInnerUses(return_instr, callsite);

  // 4. do something with inner throw
  outer_ir->exit = joinStops(outer_ir, factory, cast<StopInstr>(outer_ir->exit),
                             throw_instr);

  // 4. Replace call instruction with body of callee.
  outer_ir->inlineBlocks(callsite, start_block, return_block);
}

/**
 *
 * @param cxt JIT compilation context
 * @param ir  HMIR of a method to scan for callsites, and inline those that match policy
 * @return false if nothing inlined
 */
bool inlineGraph(Context *cxt, InstrGraph* ir, const char *title) {
  (void)title;
  assert(checkPruned(ir) && checkSSA(ir));

  String *caller_name = cxt->method->getMethodName();

  inline_enable_verbose = INLINE_ALWAYS_VERBOSE || (enable_verbose!=0);
  inline_enable_verbose |=  debugIsInSkiplist("DEBUG_INLINE_VERBOSITY_CALLER", caller_name );

  if (!dbgInlineOnly(cxt->method))
    return false; // Do not consider methods other than those listed.

  // Make an inline worklist. Can't just scan and inline as inlining munches
  // the control flow graph and the iterators get confused.  Scan and mark
  // for now, hold off on scanning the inlined code again -- at least only
  // when we know there is no recursion.
  Allocator scratch;
  SeqBuilder<InlineWorklistItem> work(scratch);

  uint32_t inlineCount = 0;
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    for (InstrRange i(b.front()); !i.empty(); i.popFront()) {
      Instr* instr = i.front();
      switch (kind(instr)) {
        case HR_callmethod: {
          CallStmt2 *call = cast<CallStmt2>(instr);
          if (isRecursiveCallsite(cxt,call)) {
            break;
          }
          if (ableToInline(cxt,call) && isCorrectToStaticallyInline(cxt, call)) {
            inlineCount++;
            work.add(InlineWorklistItem(call));
          }
          break;
        }
      }
    }
  }

  dbgPrintInlineWorklist(cxt, work);

  bool did_ir_change = false;

  //inline the call sites on work list if mgr policy says so.
  //compiling a callee may recurse (a very long way around) into inliner.
  InstrFactory factory(ir);
  for (SeqRange<InlineWorklistItem> iter(work); !iter.empty(); iter.popFront()) {
    InlineWorklistItem& item = iter.front();
    CallStmt2 *call = cast<CallStmt2>(item.call_site);
    MethodInfo* target = targetOfCallSite(call);
    if (!item.callee_ir && shouldAttemptInline(cxt, target, inlineCount)) {
      item.callee_ir = compileCallee(cxt, call); //NB. potentially recursive
      //StringBuffer buf(cxt->toplevel->core());
      //buf << "inline: " <<  target->getMethodName();
      //printf("%s\n", buf.c_str()); fflush(NULL);
      dbgInlineCallsite(call,cxt, item.callee_ir);
      inlineCallsite(ir, &factory, item.callee_ir, item); //REAL WORK
      did_ir_change = true;
    }
  }

  dbgDumpIr(ir, "after-inlining", cxt );

  pruneGraph(ir);
  assert(checkPruned(ir));

  dbgDumpIr(ir, "after-pruning", cxt );

  assert(checkSSA(ir));
  return did_ir_change;
}

//#ifdef AVMPLUS_VERBOSE #ifdef AVMPLUS_VERBOSE #ifdef AVMPLUS_VERBOSE #ifdef A
#ifdef AVMPLUS_VERBOSE

inline static void dbgDumpIr(InstrGraph *ir, const char *banner, Context *cxt) {
  if (!inline_enable_verbose)
    return;
  cxt->out << "ir for " << cxt->method->getMethodName() << banner << "\n";
  listCfg(cxt->out, ir);
  dbgPrintGraph(cxt,ir, banner);
}

/**
 * for detective work..
 * env DEBUG_INLINE_CANDIDATE set means inline only those candidates.
 * (Useful for narrowing down problems)
 * env method name listed in DEBUG_INLINE_SKIP indicates that we should never inline it.
 * (Useful for working a specific problem with a specific callee)
 */
inline static bool dbgInlineSkip(MethodInfo *method_info) {
   //when DEBUG_INLINE_CANDIDATE env is set, allow ONLY inlines of methods listed.
  const char *env = VMPI_getenv("DEBUG_INLINE_CANDIDATE");
  if (env != NULL && *env != 0) {
    return debugIsInSkiplist("DEBUG_INLINE_CANDIDATE", method_info->getMethodName());
  }
   //otherwise, if DEBUG_INLINE_SKIP contains the name, skip it.
  //use like this:
  //env DEBUG_INLINE_SKIP="`cat inline-skip-list.txt`" INLINE=1 avmshell -Dnodebugger xx.abc
  //
  return debugIsInSkiplist("DEBUG_INLINE_SKIP", method_info->getMethodName());
}

/**
 * for detective work.. env  DEBUG_INLINE_ONLY indicates
 *  that we should only consider callsites in the the named methods.
 *  returns true when env vars suggest we should skip examining callsites of the method for inlining.
 */
inline static bool dbgInlineOnly(MethodInfo *method_info){
  const char *env = VMPI_getenv("DEBUG_INLINE_ONLY");
  if (!env)
    return true; // if not set, behave normally, inline
  return debugIsInSkiplist("DEBUG_INLINE_ONLY", method_info->getMethodName());
}

/**
 * verbose output to help follow what inliner is doing.
 */
inline static void dbgInlineCallsite(CallStmt2 *call, Context *cxt, InstrGraph *callee_ir/*InlineWorklistItem& item*/){

  if (inline_enable_verbose) {
    MethodInfo* mi = targetOfCallSite(call);
    cxt->out << "Compiled IR to be inlined for " << mi << "\n";
    listCfg(cxt->out, callee_ir);
  }
 if (parseEnv("DEBUG_PRINT_INLINES", 0) != 0) {
   //print the names of methods that we are to inline
   const char *tn = type(call->param_in())->name;
   String *body = cxt->method->getMethodName();
   cxt->out << "inline: " << tn << " into: " << body << "\n";
   for (const Context *aContext = cxt->caller; aContext != NULL; aContext = aContext->caller) {
     cxt->out << "  " << aContext->method << "\n";
   }
 }
}

inline static void dbgDumpIr(Instr *call_site, InstrGraph* outer_ir, InstrGraph *callee_ir) {
  PrintWriter &pw = outer_ir->lattice.console();
  if (inline_enable_verbose) {
    pw << "===========\ninline into callsite:  ";
    printInstr(pw, call_site);
    pw << "--------outer_ir:\n";
    listCfg(pw, outer_ir);
    pw << "--------callee_ir:\n";
    listCfg(pw, callee_ir);
    pw << "============\n";
  }
}

/**
 * verbose output to dump out the work list
 */
inline static void dbgPrintInlineWorklist(Context *cxt, SeqBuilder<InlineWorklistItem>& work)
{
  //print out the worklist
  if (inline_enable_verbose) {
    if (work.isEmpty()) {
      cxt->out << "worklist empty, no callsites to inline\n";
    }
    cxt->out << "inliner worklist{";
    for (SeqRange<InlineWorklistItem> iter(work); !iter.empty(); iter.popFront()) {
      cxt->out << targetOfCallSite(iter.front().call_site)->getMethodName() << ", "; //"\n";
    }
    cxt->out << "}end inliner worklist\n";


  }
}

/*
 * print out one of our lovely gml files to  show what we did to the IR
 */
inline static void dbgPrintGraph(Context *cxt, InstrGraph *ir, const char *title){
  char file_name[512];
  static int fnum = 0;
  VMPI_snprintf(file_name, 512,"%s-%d", title, fnum++); //groan, no snprintf on windows
  cxt->out << file_name << " for gml ir view of " << cxt->method << " " << title << "\n";
  printGraph(cxt, ir, file_name);
}

#endif/*AVMPLUS_VERBOSE*/

}//namespace
#endif // VMCFG_HALFMOON
