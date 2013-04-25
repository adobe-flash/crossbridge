/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

#include "profiler/profiler-main.h"
#include "hm-deoptimizer.h"

// CallInfo definitions for functions we need to generate calls to.
#include "hm-bailout-stubs.h"

// fixme: these extern decls belong in LirHelper.h; they're here for now
// to make merging easier.  Keep each group in alphabetical order.
namespace avmplus {
extern void listing(const char* title, AvmLogControl&, Fragment*);
}

namespace halfmoon {
using namespace avmplus;
using namespace nanojit;

LirBuffer* LirHelper2::createLirout(bool verbose, const char *title)
{
  frag = new (*lir_alloc) Fragment(0 verbose_only(, 0));
  LirBuffer* lirbuf = frag->lirbuf = new (*lir_alloc) LirBuffer(*lir_alloc);
  lirbuf->abi = ABI_CDECL;
  lirout = new (*alloc1) LirBufWriter(lirbuf, core->config.njconfig);
#ifdef NJ_VERBOSE
  if (verbose || enable_gml) {
    lirbuf->printer = new (*lir_alloc) LInsPrinter(*lir_alloc, TR_NUM_USED_ACCS);
    lirbuf->printer->addrNameMap->addAddrRange(pool->core, sizeof(AvmCore), 
          0, "core");
  }
#else
  (void) verbose;
#endif

#ifdef DEBUG
  lirout = validate2 = new (*alloc1) ValidateWriter(lirout, 
    lirbuf->printer, title);
#else
  (void) title;
#endif

#ifdef NJ_VERBOSE
  if (verbose) {
    lirout = verbose_writer = new (*alloc1) VerboseWriter(*alloc1, lirout, 
          lirbuf->printer, &pool->codeMgr->log, title);
  }
#endif
  return lirbuf;
}

LIns* LirHelper2::setName(LIns* ins, const char* name) {
#ifdef NJ_VERBOSE
  if (frag->lirbuf->printer && name)
    frag->lirbuf->printer->lirNameMap->addName(ins, name);
#else
  (void) name;
#endif
  return ins;
}

/// Builds method frame and updates current frame on core* object at prologue
LIns* LirHelper2::initMethodFrame(LIns* env_param) {
  LIns* new_frame = stackAlloc(sizeof(MethodFrame), "method_frame");
  LIns* current_frame = ldp(coreAddr, JitFriend::core_cmf_offset, ACCSET_OTHER);

  stp(env_param, new_frame, JitFriend::mf_env_offset, ACCSET_OTHER);
  stp(current_frame, new_frame, offsetof(MethodFrame, next), ACCSET_OTHER);
  stp(new_frame, coreAddr, JitFriend::core_cmf_offset, ACCSET_OTHER);

#ifdef DEBUG
  stp(InsConstPtr((void*) (uintptr_t) 0xdeadbeef), 
                  new_frame, JitFriend::mf_dxns_offset, ACCSET_OTHER);
#endif

  return new_frame;
}

/// Builds pop method frame at method epilogue
void LirHelper2::popMethodFrame(LIns* method_frame) {
  LIns* next = ldp(method_frame, offsetof(MethodFrame, next), ACCSET_OTHER);
  stp(next, coreAddr, JitFriend::core_cmf_offset, ACCSET_OTHER);
}

using nanojit::Assembler;
CodeList* LirHelper2::assemble(const nanojit::Config& config,
                               HMDeoptDataWriter* mdwriter) {
  CodeMgr* codeMgr = pool->codeMgr;

#ifdef NJ_VERBOSE
  if (pool->isVerbose(LC_Liveness)) {
    Allocator live_alloc;
    LirReader in(frag->lastIns);
    nanojit::live(&in, live_alloc, frag, &codeMgr->log);
  }
#endif

  AvmAssert(halfmoon::checkLir(frag, &codeMgr->log));

  Assembler *assm =
    new (*lir_alloc) Assembler(codeMgr->codeAlloc, codeMgr->allocator,
                               *lir_alloc, &codeMgr->log, config, mdwriter);

#ifdef AVMPLUS_VERBOSE
  StringList asmOutput(*lir_alloc);
  if (!pool->isVerbose(VB_raw))
    assm->_outputCache = &asmOutput;
#endif

  assm->beginAssembly(frag);
  LirReader bufreader(frag->lastIns);
  assm->assemble(frag, &bufreader);
  CodeList* code_list = assm->endAssembly(frag);

#ifdef AVMPLUS_VERBOSE
  assm->_outputCache = 0;
  for (Seq<char*>* p = asmOutput.get(); p != NULL; p = p->tail) {
    assm->outputf("%s", p->head);
  }
#endif

  return code_list;
}

#ifdef AVMPLUS_ARM
#ifdef _MSC_VER
#define RETURN_METHOD_PTR(_class, _method) \
return *((int*)&_method);
#else
#define RETURN_METHOD_PTR(_class, _method) \
union { \
    int (_class::*bar)(); \
    int foo[2]; \
}; \
bar = _method; \
return foo[0];
#endif

#elif defined __GNUC__
#define RETURN_METHOD_PTR(_class, _method)      \
  union {                                       \
    int (_class::*bar)();                       \
    intptr_t foo;                               \
  };                                            \
  bar = _method;                                \
  return foo;
#else
#define RETURN_METHOD_PTR(_class, _method)      \
  return *((intptr_t*)&_method);
#endif

#ifdef _MSC_VER
    #if !defined (AVMPLUS_ARM) || defined(UNDER_RT)
    extern "C"
    {
        int __cdecl _setjmp3(jmp_buf jmpbuf, int arg);
    }
    #else
    #include <setjmp.h>
    #undef setjmp
    extern "C"
    {
        int __cdecl setjmp(jmp_buf jmpbuf);
    }
    #endif // AVMPLUS_ARM
#endif // _MSC_VER
  
#if defined _MSC_VER && !defined AVMPLUS_ARM
#  define SETJMP ((uintptr_t)_setjmp3)
#elif defined AVMPLUS_MAC_CARBON
#  define SETJMP setjmpAddress
#else
#  define SETJMP ((uintptr_t)VMPI_setjmpNoUnwind)
#endif // _MSC_VER
  
#define FUNCADDR(addr) (uintptr_t)addr
#define EFADDR(f)   efAddr((int (ExceptionFrame::*)())(&f))
  
  intptr_t efAddr( int (ExceptionFrame::*f)() )
  {
    RETURN_METHOD_PTR(ExceptionFrame, f);
  }
  

  int32_t hmBeginCatch(AvmCore* core,
                       ExceptionFrame* ef,
                       MethodInfo* info,
                       intptr_t pc,
                       AnyVal* slotPtr)
  {
    int32_t ordinal;
    ef->beginCatch();
    Exception* exception = core->exceptionAddr;
    ExceptionHandler* handler = core->findExceptionHandlerNoRethrow(info, pc, exception, &ordinal);
    if (!handler) {
      // No matching exception, so rethrow.
      core->throwException(exception);
    }
    ef->beginTry(core);
    slotPtr->atom = exception->atom;

    return ordinal;
  }

  void hmBeginTry(ExceptionFrame* ef, AvmCore* core) {
    ef->beginTry(core);
  }
  void hmEndTry(ExceptionFrame* ef) {
    ef->endTry();
  }

  FUNCTION(FUNCADDR(hmBeginCatch), SIG5(I,P,P,P,P,P), hmBeginCatch)
  
  FUNCTION(SETJMP, SIG2(I,P,I), fsetjmp)
  METHOD(FUNCADDR(hmBeginTry), SIG2(V,P,P), hmBeginTry)
  METHOD(FUNCADDR(hmEndTry), SIG1(V,P), hmEndTry)
  

/// Types understood by LIR.  We don't use LTy here because it conflates
/// LTy_P with either LTy_I or LTy_Q, but we want to distinguish them.
enum LirModel {
  kLirPtr,
  kLirInt,
  kLirDouble,
  kLirVoid
};

/// Return the LIR data type to use for the given Type.
///
LirModel lirtype(const Type* t) {
  if (isBottom(t))
    return kLirVoid;
  if (isDataType(*t)) {
    switch (model(t)) {
      default: assert(false && "bad model");
      case kModelAtom:
      case kModelScriptObject:
      case kModelString:
      case kModelNamespace:
        return kLirPtr;
      case kModelInt:
        return kLirInt;
      case kModelDouble:
        return kLirDouble;
    }
  }
  switch (kind(t)) {
    case kTypeOrdinal: return kLirInt;
    default: return kLirPtr;
  }
}

// walk the IR graph top-down, generating LIR instructions for each Instr.
//
// we will need to remeber at least one LIns* for each def.  more than one
// will be needed if we have separate tag values and real values.
//
// nanojit doesn't support phis.  we will need to create stack space, store
// at block ends, then load at block starts, or else do something more clever.
// one option would be to resurrect bill's phi work.
//
// It would kind of suck if we have to write lots of C++ code that generates
// lots of LIR code.  Better would be if we have snips of LIR sitting around
// that we can patch together.  would they come from lirasm?
//
// Outline of CodegenLIR:
//
// prolog:
//   LIR_start
//   LIR_params
//   init MethodFrame
//     mf = alloc sizeof(MethodFrame)
//     cmf = ldp(core, offsetof(currentMethodFrame))
//     stp(env, mf,  offsetof(envOrCodeContext))
//     stp(cmf, mf,  offsetof(next))
//     stp(mf, core, offsetof(currentMethodFrame)
//   alloc vars & tags [done by HR_newstate]
//   checkstack [TODO: add HR_checkstack]
//   alloc ExceptionFrame (if exceptions)
//   alloc CallStackNode (if debug)
//   init optional args
//   init required args
//   unbox this [automatic when param0 is SST_atom]
//   init rest or arguments, maybe lazy
//
// body:
//   init fields (if ctor)
//   call debugEnter (if debug)
//   call beginTry+setjmp (if exceptions)
//
// return:
//   call debugExit (if debug)
//   call endTry (if exceptions)
//   destroy methodFrame
//   LIR_ret
//
// If env were available in the IR, then dependent things like VTable,
// Traits, AvmCore, etc, could be loaded from it, and these ops could be
// scheduled.
//
// REST/ARGUMENTS could be done as a tuple of (count, ap, array), with
// optimized helpers taking & returning tuple values, and we try to stack
// allocate and pass by reference underneath.
//
// how to model optional parameters?

LirEmitter::LirEmitter(Context* cxt, InstrGraph* ir, 
                      ProfiledInformation* profiled_info)
: LirHelper2(cxt->method->pool())
, cxt(cxt)
, signature(cxt->method->getMethodSignature())
, ir(ir)
, env_param(0)
, argc_param(0)
, ap_param(0)
, loop_live_(*alloc1)
, npe_label(0)
, upe_label(0)
, interrupt_label(0)
, safepoint_space_(NULL)
, get_cache_builder(*alloc1, *pool->codeMgr)
, set_cache_builder(*alloc1, *pool->codeMgr)
, call_cache_builder(*alloc1, *pool->codeMgr)
, enable_verbose_lir_(enable_verbose && !enable_trace) 
, profiled_info_(profiled_info)
, bailout_branches_(*alloc1)
, have_safepoints(false)
, have_catchblocks_(false)
, emittedBeginCatch(false)
, catchLabels(NULL)
{
  allocateTables();
}

LirEmitter::~LirEmitter() {
}

/**
 * EachHotBlocks visits blocks in preorder, hottest block first.  This is
 * an attempt to cuddle locate long sequences of straightline code.
 */
class EachHotBlock {
public:
  explicit EachHotBlock(InstrGraph* ir, ProfiledInformation* profiled_info = NULL);

public:
  // Range api
  bool empty() const {
    return iter.empty();
  }
  BlockStartInstr* front() const {
    assert(!empty());
    return iter.front();
  }
  BlockStartInstr* popFront() {
    assert(!empty());
    return iter.popFront();
  }

private:
  void dfsPreOrder(BlockStartInstr* block, ProfiledInformation* profiled_info);

private:
  explicit EachHotBlock(const EachHotBlock&); // do not implement.

private:
  Allocator scratch;
  InstrGraph* ir;
  BitSet visited; // one bit per block, indexed by blockid
  SeqBuilder<BlockStartInstr*> blocks;
  SeqRange<BlockStartInstr*> iter;
};

EachHotBlock::EachHotBlock(InstrGraph* ir, ProfiledInformation* profiled_info)
: ir(ir), visited(scratch, ir->block_count()), blocks(scratch), iter(0)
{
  dfsPreOrder(ir->begin, profiled_info);
  this->iter = SeqRange<BlockStartInstr*>(blocks.get());
}

/***
 * We linearize the graph based on profiling information.
 * We always visit the hottest branch first and add it to the beginning
 * of the list, which is effectively a preordering with a DFS.
 */
void EachHotBlock::dfsPreOrder(BlockStartInstr* block,
                               ProfiledInformation* profiled_info) {
if (visited.get(block->blockid))
  return;
visited.set(block->blockid);
blocks.add(block);

if (ir->hasBlockEnd(block)) {
  Instr* end = ir->blockEnd(block);
  InstrKind k = kind(end);
  // recurse into successors
  if (k == HR_goto) {
    dfsPreOrder(cast<GotoInstr>(end)->target, profiled_info);
  } else if (k == HR_if || k == HR_switch) {
    CondInstr* block_end = (CondInstr*) end;
    assert(block_end->armc == 2);
    ArmInstr* firstArm = block_end->arms[0];
    ArmInstr* secondArm = block_end->arms[1];

    if (profiled_info->isHotterArm(firstArm, secondArm)) {
      for (ArrayRange<ArmInstr*> r = armRange((CondInstr*)end); !r.empty();)
        dfsPreOrder(r.popFront(), profiled_info);
    } else {
      for (ArrayRange<ArmInstr*> r = armRange((CondInstr*)end); !r.empty();)
        dfsPreOrder(r.popBack(), profiled_info);
    }
  } else if (k == HR_return || k == HR_throw) {
    // no successors
  } else {
      assert(false && "unsupported block-end opcode");
  }
}
}

/// Sort the IR blocks in linear order.  All LIR generation passes will use
/// this order for liveness hints and fall-through branch elimination.
/// TODO: find loops and ensure loop blocks are contiguous.
///
void LirEmitter::sortBlocks() {
  Allocator scratch;
  int count = 0;
  SeqBuilder<BlockStartInstr*> list(scratch);

  if (false /* enable_profiler */) {
    for (EachHotBlock b(ir, profiled_info_); !b.empty(); ++count)
      list.add(b.popFront());
  } else {
    // just use reverse postorder
    for (EachBlock b(ir); !b.empty(); ++count)
      list.add(b.popFront());
  }

  num_blocks_ = count;
  blocks_ = new (*alloc1) BlockStartInstr*[count];
  count = 0;
  for (SeqRange<BlockStartInstr*> i(list); !i.empty(); i.popFront()) {
    blocks_[count++] = i.front();
  }
  analyzeLiveness();
}

/// Liveness information for one block.
///
struct BlockInfo {
  BlockInfo(Allocator& alloc, int ir_size) :
      loop_end(0), live(alloc, ir_size) {
  }

  // Latest predecessor in linear order that jumps to this block.
  Instr* loop_end;

  // Bitmask of live instructions at the start of this block.
  BitSet live;
};

/// Merge the live bits for a target block with the current bits.  If this
/// is the first time we've seen that target, record 'from' as the loop_end.
/// The edge (from -> target) is considered the bottom-most loop edge.
///
void analyzeEdgeLiveness(HashMap<Instr*, BlockInfo*> &livemap,
                         BitSet& live,
                         Instr* from,
                         Instr* target,
                         Allocator& scratch,
                         int ir_size) {
  if (livemap.containsKey(target)) {
    live.setFrom(livemap.get(target)->live);
  } else {
    BlockInfo* target_info = new (scratch) BlockInfo(scratch, ir_size);
    target_info->loop_end = from;
    livemap.put(target, target_info);
  }
}

/// Analyze liveness as follows.  We consider every instruction to be
/// useful.  Our goal is to find instructions live at loop labels, and
/// identify the latest branch instruction to each loop label.
///
/// For each block, bottom up, as long as live sets change:
///   1. start with an empty live set.
///   2. union the live sets from successors
///   3. mark any first-seen successors as loops, record this branch
///      as the loop branch.  (we will use this later to insert LIR_live).
///   4. iterate each instruction bottom up: remove the instruction from live,
///      then add any instructions used by this one.  This traversal includes
///      block-ends with uses as well as block-starts with defs.
///   5. save the live set.
///
/// For each loop block:
///   1. save a list of live instructions at the start of the block, and
///      save it in the loop_live_ HashMap.
///
/// Limitations:
///   * this design iterates too much; it visits every block, when only
///     the changed ones need to be revisited.
///   * If we count on the bocks being in reverse postorder, then do we
///     really need to iterate?  just knowing what is live at loop entry
///     is enough to extend live ranges with LIR_live.
///   * live sets are bitmaps indexed by instr->id.  It would be better
///     to track defs, not instructions, and use a less-sparse set.
///
void LirEmitter::analyzeLiveness() {
  Allocator scratch;
  BitSet live(scratch, ir->size());
  HashMap<Instr*, BlockInfo*> livemap(scratch, num_blocks_);

  bool changed;
  do {
    changed = false;
    for (ArrayRange<BlockStartInstr*> b(blocks_, num_blocks_); !b.empty();
        b.popBack()) {
      BlockStartInstr* block = b.back();
      live.reset();
      Instr* branch = ir->blockEnd(block);
      switch (kind(branch)) {
        case HR_goto: {
          // Pick up live bits from the target
          GotoInstr* go = cast<GotoInstr>(branch);
          analyzeEdgeLiveness(livemap, live, go, go->target, scratch,
                              ir->size());
          break;
        }
        case HR_if:
        case HR_switch: {
          // Pick up live bits from each arm
          for (ArrayRange<ArmInstr*> a = armRange((CondInstr*)branch); !a.empty();)
            analyzeEdgeLiveness(livemap, live, branch, a.popFront(), scratch,
                                ir->size());
          break;
        }
        default:
          break;
      }
      if (InstrGraph::blockEnd(block)->catch_blocks != NULL) {
        for (CatchBlockRange cb(block); !cb.empty();) {
          analyzeEdgeLiveness(livemap, live, branch, cb.popFront(), scratch,
                              ir->size());
        }
      }
      for (InstrRange i(block); !i.empty(); i.popBack()) {
        Instr* instr = i.back();
        live.clear(instr->id);
        for (ArrayRange<Use> a = useRange(instr); !a.empty(); a.popFront())
          live.set(definerId(a.front()));
      }
      BlockInfo* block_info = livemap.get(block);
      if (!block_info) {
        block_info = new (scratch) BlockInfo(scratch, ir->size());
        livemap.put(block, block_info);
      }
      changed |= block_info->live.setFrom(live);
    }
  } while (changed);

  have_loop_ = false;

  // For each loop header, save the list of live instructions on entry
  // to that loop, so we can insert LIR_live instructions later.
  for (ArrayRange<BlockStartInstr*> b(blocks_, num_blocks_); !b.empty();
      b.popBack()) {
    Instr* block = b.back();
    BlockInfo* old_info = livemap.get(block);
    if (!old_info || !old_info->loop_end)
      continue; // not a loop header.
    have_loop_ = true;
    Seq<Instr*> *live_instrs = 0;
    // fixme: visiting all instrs is sad!
    for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
      Instr* instr = i.front();
      if (old_info->live.get(instr->id))
        live_instrs = cons(*alloc1, instr, live_instrs);
    }
    loop_live_.put(block, live_instrs);
    if (enable_verbose_lir_ && live_instrs) {
      // print out the list.
      printf("live B%d:", block->id);
      for (SeqRange<Instr*> i(live_instrs); !i.empty(); i.popFront())
        printf(" %s%d", kInstrPrefix, i.front()->id);
      printf("\n");
    }
  }
}

/// Allocate tables we need during LIR code generation.
///
void LirEmitter::allocateTables() {
  assert(pool->codeMgr && "CodeMgr not initialized yet");
  Allocator0 alloc0(*alloc1);

  // Sort all the blocks.  Later passes will use this order.
  sortBlocks();

  // Fill in the val_offsets and dom_children arrays.
  int num_defs = ir->def_count();
  int max_argc = 0;
  for (ArrayRange<BlockStartInstr*> b(blocks_, num_blocks_); 
        !b.empty(); b.popFront()) {

    if (kind(b.front()) == HR_catchblock) {
      have_catchblocks_ = true;
      if (catchLabels == NULL) {
        catchLabels = new (*lir_alloc) HashMap<intptr_t,CatchBlock*>(*lir_alloc, 4);
      }
      CatchBlockInstr* cblock = cast<CatchBlockInstr>(b.front());
      catchLabels->put(cblock->vpc, new (*lir_alloc) CatchBlock(cblock));
    }

    for (InstrRange r(b.front()); !r.empty(); r.popFront()) {
      max_argc = max(max_argc, numUses(r.front()));
    }
  }

  // Table of defs, indexed by val_offsets[instr->id] + pos(def)
  // DEOPT: The def_ins_ array must survive to the end of assembly.
  def_ins_ = new (alloc0) LIns*[num_defs];
  instr_ins = new (alloc0) LIns*[ir->size()]; // fixme: too sparse!
  max_argc_  = max_argc;
}

/// Set the LIns* associated with a specific Def.
///
inline LIns* LirEmitter::set_def_ins(Def* d, LIns* ins) {
#ifdef NJ_VERBOSE
  if (enable_verbose_lir_) {
    StringBuffer b(core);
    printDef(b, d);
    setName(ins, b.c_str());
  }
#endif

  return def_ins_[defId(d)] = ins;
}

/// Get the LIns* for a specific Def.
///
inline LIns* LirEmitter::def_ins(const Def* d) {
  return def_ins_[defId(d)];
}

/// Get the LIns* for a specific Def, given any Use of that Def.
///
inline LIns* LirEmitter::def_ins(const Use& u) {
  return def_ins(def(u));
}

/// Set the LIns* for a specific instruction (not Def).
///
inline LIns* LirEmitter::set_ins(Instr* i, LIns* ins) {
#ifdef NJ_VERBOSE
  if (enable_verbose_lir_) {
    StringBuffer b(core);
    b << kInstrPrefix << i->id;
    setName(ins, b.c_str());
  }
#endif

  return instr_ins[i->id] = ins;
}

/// Get the LIns* for a specific instruction (not Def)
///
inline LIns* LirEmitter::ins(Instr* i) {
  return instr_ins[i->id];
}

void LirEmitter::emitStackOverflowCheck() {
  // stack overflow check - copied from codegenlir
  assert (method_frame_ != NULL);
  LIns* minstack  = ldp(coreAddr, JitFriend::core_minstack_offset, ACCSET_OTHER);
  LIns* compare_stack = lirout->ins2(LIR_ltup, method_frame_, minstack);

  /***
   * if overflow() { call handleOverflow(); } 
   */
  LIns* stack_overflow = lirout->insBranch(LIR_jf, compare_stack, 0);
  callIns(&ci_handleStackOverflowMethodEnv, 1, env_param);
  LIns* begin_label = setName(label(), "begin");
  stack_overflow->setTarget(begin_label);
}

void LirEmitter::emitBegin(bool has_reachable_exceptions) {
#ifdef NJ_VERBOSE
  bool verbose = enable_verbose_lir_ || pool->isVerbose(VB_jit);
#else
 bool verbose = enable_verbose_lir_; 
#endif

  LirBuffer* lirbuf = createLirout(verbose, "LIR");
  // add other LirWriters here.
#ifdef NJ_VERBOSE
  if (verbose_writer)
    verbose_writer->never_flush = true;
#endif

  emitStart(*alloc1, lirbuf, lirout);
  env_param = param(0, "env");
  argc_param = setName(p2i(param(1)), "argc");
  ap_param = param(2, "ap");
  coreAddr = setName(InsConstPtr(pool->core), "core"); // TODO: expose in HR?
  method_frame_ = initMethodFrame(env_param);
  emitStackOverflowCheck();
  args_ = stackAlloc(max_argc_ << VARSHIFT(cxt->method), "args");

  // Create a buffer for epilog code that we can write into in parallel.
  LirBuffer* epilog_buf = new (*lir_alloc) LirBuffer(*lir_alloc);
  epilog_buf->abi = ABI_CDECL;
#ifdef NJ_VERBOSE
  epilog_buf->printer = lirbuf->printer;
#endif
  traps_lir = new (*alloc1) LirBufWriter(epilog_buf, core->config.njconfig);
  traps_skip = traps_lir->insSkip(0);

  // then space for the exception frame, be safe if its an init stub
  if (has_reachable_exceptions) {
    // [_save_eip][ExceptionFrame]
    // offsets of local vars, rel to current ESP
    _save_eip = stackAlloc(sizeof(intptr_t), "_save_eip");
    _ef       = stackAlloc(sizeof(ExceptionFrame), "_ef");
    MethodSignaturep method_signature = signature;
    int alloc_size = method_signature->frame_size();
    safepoint_space_ = stackAlloc(alloc_size << VARSHIFT(cxt->method),
                                  "deopt locals");
  } else {
    _save_eip = NULL;
    _ef = NULL;
    safepoint_space_ = NULL;
  }
}

LIns* LirEmitter::emitConst(const Type* t) {
  assert(isConst(t));
  switch (kind(t)) {
    default:
      assert(false && "unsupported kind");
    case kTypeName:
      return InsConstPtr(nameVal(t));
    case kTypeOrdinal:
      return InsConst(ordinalVal(t));
    case kTypeTraits:
      return InsConstPtr(traitsVal(t));
    case kTypeMethod:
      return InsConstPtr(methodVal(t));
    case kTypeAny:
    case kTypeObject:
    case kTypeVoid:
    case kTypeString:
    case kTypeNamespace:
    case kTypeBoolean:
    case kTypeNumber:
    case kTypeScriptObject:
      switch (model(t)) {
        default:
          assert(false && "unsupported model");
        case kModelScriptObject:
          return InsConstPtr(objectVal(t));
        case kModelNamespace:
          return InsConstPtr(nsVal(t));
        case kModelString:
          return InsConstPtr(stringVal(t));
        case kModelAtom: {
          Atom const_atom = atomVal(t, core);
          // Numbers and strings can be gc allocated
          // So we have to store them in the pool to keep them alive
          if (AvmCore::isDouble(const_atom)) {
            pool->cpool_const_double.add((GCDouble*)atomPtr(const_atom));
          } else if (AvmCore::isString(const_atom)) {
            pool->cpool_const_string.add((String*)atomPtr(const_atom));
          }
          return InsConstAtom(const_atom);
        }
        case kModelDouble:
          return lirout->insImmD(doubleVal(t));
        case kModelInt:
          assert(isInt(t) || isUInt(t) || isBoolean(t));
          return InsConst(isInt(t) ? intVal(t) :
                          isUInt(t) ? (int32_t)uintVal(t) :
                          (int32_t)boolVal(t));
      }
  }
}

LIns* LirEmitter::emitAvmCall(const Use* args_in, int actual_argc,
                              MethodSignaturep callee_sig, LIns* callee_env,
                              Def* value_out) {
  assert(isBottom(type(value_out)) ||
         model(type(value_out)) == defaultModelKind(callee_sig->returnTraits()));
  emitStoreArgs(args_in, actual_argc, callee_sig);
  LIns* result = emitIndirectAvmCall(type(value_out), callee_env, actual_argc,
                                     args_);
  return result;
}

LIns* LirEmitter::emitIndirectAvmCall(const Type* result_type, LIns* callee_env,
                                      int actual_argc, LIns* args) {
  LIns* addr = ldp(callee_env, JitFriend::env_impl_offset, ACCSET_OTHER);
  // AVM calling convention doesn't count the receiver parameter.
  LIns* argc_ins = InsConst(actual_argc - 1);
  ModelKind k = isBottom(result_type) ? kModelInvalid : model(result_type);
  switch (k) {
    default:
      assert(false && "unknown model");
    case kModelInvalid: // result type is UN.
    case kModelAtom:
    case kModelString:
    case kModelNamespace:
    case kModelScriptObject:
      return callIns(&ci_acalli, 4, addr, callee_env, argc_ins, args);
    case kModelInt:
      return callIns(&ci_icalli, 4, addr, callee_env, argc_ins, args);
    case kModelDouble:
      return callIns(&ci_dcalli, 4, addr, callee_env, argc_ins, args);
  }
}

LIns* LirEmitter::emitInterfaceAvmCall(const Type* result_type, LIns* callee_env,
                                      int actual_argc, LIns* args, LIns* iid) {
  LIns* addr = ldp(callee_env, JitFriend::env_impl_offset, ACCSET_OTHER);
  // AVM calling convention doesn't count the receiver parameter.
  LIns* argc_ins = InsConst(actual_argc - 1);
  ModelKind k = isBottom(result_type) ? kModelInvalid : model(result_type);
  switch (k) {
    default:
      assert(false && "unknown model");
    case kModelInvalid: // result type is UN.
    case kModelAtom:
    case kModelString:
    case kModelNamespace:
    case kModelScriptObject:
      return callIns(&ci_acallimt, 5, addr, callee_env, argc_ins, args, iid);
    case kModelInt:
      return callIns(&ci_icallimt, 5, addr, callee_env, argc_ins, args, iid);
    case kModelDouble:
      return callIns(&ci_dcallimt, 5, addr, callee_env, argc_ins, args, iid);
  }
}

/// Store argc + 1 atoms to a stack allocated buffer.  Based on code from
/// CodegenLIR::storeAtomArgs()
///
void LirEmitter::emitAtomArgs(const Use* args_in, int actual_argc) {
  for (int i = 0; i < actual_argc; ++i)
    stp(def_ins(args_in[i]), args_, i * sizeof(Atom), ACCSET_OTHER);
}

void LirEmitter::emitStoreArgs(const Use* args_in, int actual_argc,
                               MethodSignaturep callee) {
  // FIXME: pad alignment if first double arg is unalinged, 32bit-only
  ParamIter p(callee);
  for (int i = 0; i < actual_argc; ++i, p.popFront()) {
    assert(p.disp() < int(max_argc_ << VARSHIFT(cxt->method)));
    LIns* arg_ins = def_ins(args_in[i]);
    switch (p.builtinType()) {
      case BUILTIN_number:
        std(arg_ins, args_, p.disp(), ACCSET_OTHER);
        break;
      case BUILTIN_int:
        stp(i2p(arg_ins), args_, p.disp(), ACCSET_OTHER);
        break;
      case BUILTIN_uint:
      case BUILTIN_boolean:
        stp(ui2p(arg_ins), args_, p.disp(), ACCSET_OTHER);
        break;
      default:
        stp(arg_ins, args_, p.disp(), ACCSET_OTHER);
        break;
    }
  }
}

LIns* LirEmitter::emitLoadVTable(const Use& object) {
  assert(!isNullable(type(object)));
  assert(model(type(object)) == kModelScriptObject);
  return ldp(def_ins(object), offsetof(ScriptObject, vtable), ACCSET_OTHER,
             LOAD_CONST);
}

LIns* LirEmitter::emitLoadMethodEnv(LIns* vtable, int disp_id) {
  return ldp(vtable, offsetof(VTable, methods) + disp_id * sizeof(MethodEnv*),
             ACCSET_OTHER, LOAD_CONST);
}

LIns* LirEmitter::emitCompare(LIns* lhs_in, LIns* rhs_in) {
  return callIns(&ci_compare, 2, lhs_in, rhs_in);
}

LIns* LirEmitter::emitJump(LIns* target) {
  return lirout->insBranch(LIR_j, NULL, target);
}

LIns* LirEmitter::emitLoad(const Type* value_type, LIns* ptr, int32_t offset,
                           AccSet accSet, LoadQual qual) {
  switch (lirtype(value_type)) {
    default: assert(false && "bad lirtype");
    case kLirPtr: return ldp(ptr, offset, accSet, qual);
    case kLirInt: return ldi(ptr, offset, accSet, qual);
    case kLirDouble: return ldd(ptr, offset, accSet, qual);
  }
}

/**
 * Generate a LIR store instruction appropriate for the given type constraint.
 * This function does not do anything with write barriers.  Inlined setslots
 * or assignments to containers (Vector?) must handle write barriers.
 */
LIns* LirEmitter::emitStore(const Use& value, const Type* constraint,
                            LIns* ptr, int32_t offset, AccSet acc_set) {
  switch (lirtype(constraint)) {
    default: assert(false && "bad lirtype");
      // regress/bug_638233.abc - callprop on a null object
      // cknull on a null -> bottom, which has lirvoid type.
      // unsure how to fix. See halfmoon/test/sanity/acceptance/bug_638233.as
    case kLirPtr: return stp(def_ins(value), ptr, offset, acc_set);
    case kLirInt: return sti(def_ins(value), ptr, offset, acc_set);
    case kLirDouble: return std(def_ins(value), ptr, offset, acc_set);
  }
}

/** Generate a LIR return instruction for this method's return type. */
LIns* LirEmitter::emitReturn(const Use& value) {
  LIns* ins = def_ins(value);
  switch (signature->returnTraitsBT()) {
    case BUILTIN_number:  return retd(ins);
    case BUILTIN_int:     return retp(i2p(ins));
    case BUILTIN_boolean:
    case BUILTIN_uint:    return retp(ui2p(ins));
    default:              return retp(ins);
  }
}

LOpcode ltyToLiveOpcode(LTy ty) {
  switch (ty) {
    default:
      assert(false && "bad LTy");
    case LTy_I:
      return LIR_livei;
    case LTy_D:
      return LIR_lived;
#ifdef NANOJIT_64BIT
    case LTy_Q:
      return LIR_liveq;
#endif
  }
}

void LirEmitter::emitLive(Def* d) {
  LIns* ins = def_ins(d);
  if (ins)
    lirout->ins1(ltyToLiveOpcode(retTypes[ins->opcode()]), ins);
}

/// Generate LIR_live hint instructions for values that are live
/// at the targets of any back-edges of branch.
///
void LirEmitter::emitLiveHints(Instr* target_block) {
  Seq<Instr*>* live_instrs = loop_live_.get(target_block);
  for (SeqRange<Instr*> i(live_instrs); !i.empty(); i.popFront()) // for each live instr
    for (ArrayRange<Def> d = defRange(i.front()); !d.empty(); d.popFront()) //   for each def
      emitLive(&d.front());
}

/// Generate a LIR_label instruction for block, and give it a name.
///
LIns* LirEmitter::emitLabel(BlockStartInstr* block) {
  StringBuffer name(core);
  name << halfmoon::label(block) << block->id;
  return setName(label(), name.c_str());
}

LIns* LirEmitter::emitCatchLabel(CatchBlockInstr* block) {
  StringBuffer name(core);
  name << halfmoon::label(block) << block->id;
  LIns* catchLabel = label();
  CatchBlock* cb = catchLabels->get(block->vpc);
  cb->jmp->setTarget(catchLabel);
  return setName(catchLabel, name.c_str());
}

/// Patches all points that speculate to jump to bailout point
void LirEmitter::patchBailouts() {
  AvmAssert(false && "Unimplemented");

  // LIns* bailout_label = setName(label(), "Deopt Point");
  // for (SeqRange<LIns*> branches(bailout_branches_); !branches.empty();)
  //   branches.popFront()->setTarget(bailout_label);

  // LIns* abc_pc = ldi(vpc_space_, 0, ACCSET_OTHER, LOAD_NORMAL);
  
  // bool returns_double = signature->returnTraitsBT() == BUILTIN_number;
  // const CallInfo* deopt_call = returns_double ?
  //         FUNCTIONID(fprBailout) : FUNCTIONID(gprBailout);

  // LIns* return_val = callIns(deopt_call, 5,
  //                            coreAddr, env_param, abc_pc,
  //                            safepoint_space_, safepoint_tags_);

  // popMethodFrame(method_frame_);
  // if (returns_double)
  //   retd(return_val);
  // else
  //   retp(return_val);
}

GprMethodProc LirEmitter::finish(DeoptData** deopt_data) {
  if (!bailout_branches_.isEmpty())
    patchBailouts();

  livep(args_);
  livep(coreAddr);
  if (safepoint_space_ != NULL) {
    livep(safepoint_space_);
  }
  if (catchLabels != NULL) {
    livep(_ef);
    livep(_save_eip);
  }

  livep(method_frame_);
  LIns* last_ins = livep(env_param);

  if (npe_label || upe_label || interrupt_label) {
    // Link traps lirbuf to main lirbuf.
    traps_skip->initLInsSk(last_ins);
    traps_lir->ins1(LIR_retp, traps_lir->insImmP(0));
    traps_lir->ins1(LIR_livep, coreAddr);
    last_ins = traps_lir->ins1(LIR_livep, env_param);
  }
  frag->lastIns = last_ins;

#ifdef NJ_VERBOSE
  if (enable_verbose_lir_) {
    verbose_writer->flush();
    cxt->out << "Assemble\n";
  }

  if (enable_gml)
    printLirCfg(cxt->method, frag);

  if (enable_verbose_lir_)
    listing("LirEmitter", pool->codeMgr->log, frag);
#endif

  HMDeoptDataWriter* mdwriter = 0;
  if (enable_deopt) {
    CodeAlloc& meta_alloc = pool->codeMgr->jit_mgr->meta_alloc();
    mdwriter = new (*lir_alloc) HMDeoptDataWriter(meta_alloc, *lir_alloc,
                                                  signature, def_ins_);
  }
  *deopt_data = 0;
  CodeList* code_list = assemble(core->config.njconfig, mdwriter);
  if (code_list) {
    //pool->codeMgr->codeMemMap.put(cxt->method, code_list);
    if (mdwriter)
      *deopt_data = mdwriter->finish();
    return (GprMethodProc) frag->code();
  }
  // Assembler has called mdwriter->abandon()
  return 0;
}


/// Generate code as follows:
/// 1. create LirWriter pipelines in emitBegin()
/// 2. For each block in forward order, emit LIR for each instruction.
/// 3. assemble LIR and return a pointer to the machine code, in finish().
///

GprMethodProc LirEmitter::emit(DeoptData** deopt_data) {
#ifdef NJ_VERBOSE
  if (enable_verbose_lir_) {
    cxt->out << "Generate LIR " << cxt->method << "\n";
  }
#endif

  emitBegin(have_catchblocks_);

  // The start block is emitted specially
  current_block_ = 0;
  for (InstrRange r(blocks_[0]); !r.empty(); r.popFront()) {
    if (have_catchblocks_ && kind(r.front()) == HR_goto) {
      // Initiailize the eip
      stp(InsConstPtr((void*)-1), _save_eip, 0, ACCSET_OTHER);

      // _ef.beginTry(core);
      callIns(FUNCTIONID(hmBeginTry), 2, _ef, coreAddr);
      
      // Exception* setjmpResult = setjmp(_ef.jmpBuf);
      // ISSUE this needs to be a cdecl call
      LIns* jmpbuf = lea(offsetof(ExceptionFrame, jmpbuf), _ef);
      LIns* setjmpResult = callIns(FUNCTIONID(fsetjmp), 2, jmpbuf, InsConst(0));
      
      // If (setjmp() != 0) goto catch dispatcher, which we generate in the epilog.
      // Note that register contents following setjmp return via longjmp are not predictable.
      catch_branch = lirout->insBranch(LIR_jf, eqi0(setjmpResult), NULL);
    }
    emit(r.front());
  }

  for (int b = 1, n = num_blocks_; b < n; ++b) {
    current_block_ = b;
    for (InstrRange r(blocks_[b]); !r.empty(); r.popFront()) {
      emit(r.front());
    }
  }

  return finish(deopt_data);
}

/// Called by JIT code to trace execution of this IR instruction.
///
static void traceInstr(PrintWriter* out, Instr* instr) {
#ifdef NJ_VERBOSE
  (*out) << "X ";
  printInstr(*out, instr);
#else
  (void) out;
  (void) instr;
#endif
}

// CallInfo record for traceInstr().
static const nanojit::CallInfo ci_printinstr = {
  (uintptr_t)&traceInstr,
  CallInfo::typeSig2(ARGTYPE_V, ARGTYPE_P, ARGTYPE_P),
  ABI_CDECL,
  0, // isPure
  ACCSET_ALL
#ifdef NJ_VERBOSE
  ,"traceInstr"
#endif
};

void LirEmitter::printInstr(Instr* instr) {
  if (enable_verbose_lir_) {
#ifdef NJ_VERBOSE
    if (verbose_writer)
      verbose_writer->flush();
    cxt->out << "\n";
    halfmoon::printInstr(cxt->out, instr);
#endif
  }
  if (enable_trace) {
    if (isBlockStart(instr))
      return;
    // Generate a call to traceInstr() for this instr.
    callIns(&ci_printinstr, 2, InsConstPtr(&cxt->out), InsConstPtr(instr));
  }
}

bool LirEmitter::haveStub(InstrKind k) {
  return k < Stubs::stub_count && lir_table[k]._address;
}

void LirEmitter::emit(Instr* instr) {
  printInstr(instr);
  InstrKind k = kind(instr);

  if ( debug_stubs && haveStub(k) && debugStub(instr)) {
    do_stub(instr, &lir_table[k]);
    return;
  }

  if (enable_mode == kModeLirStubs && haveStub(k))
    do_stub(instr, &lir_table[k]);
  else
    do_instr(this, instr);
}

void LirEmitter::do_default(Instr* instr) {
  InstrKind k = kind(instr);
  if (haveStub(k))
    return do_stub(instr, &lir_table[k]);
  if (enable_verbose_lir_) {
    cxt->out << "LIR    --------\n";
    printInstr(instr);
    cxt->out << "LIR    --------\n";
    fflush(NULL);
  }
  assert(false && "unsupported instruction");
}

/// Gives ARGTYPE required for callins 
ArgType argtype(const Type* t) {
  if (isDataType(*t)) {
    switch (model(t)) {
      default: assert(false && "bad model");
      case kModelAtom:
      case kModelString:
      case kModelScriptObject:
      case kModelNamespace:
        return ARGTYPE_P;
    }
  } else {
    switch (kind(t)) {
      default: assert(false && "bad type kind");
      case kTypeBottom:
        return ARGTYPE_V;
      case kTypeOrdinal:
        return ARGTYPE_I;
      case kTypeTraits:
        return ARGTYPE_P;
    }
  }
}

/// Generate a call to a Stub function
///
void LirEmitter::do_stub(Instr* instr, const CallInfo* call) {
  LIns* args[MAXARGS];
  uint32_t argc = 0;
  uint32_t varargc = 0;
  args[argc++] = method_frame_;
  InstrKind k = kind(instr);
  int fixc = stub_fixc[k];
  uint32_t limit = (fixc == -1) ? MAXARGS : fixc + 1; // +1 is for the MethodFrame* arg
  int offset = 0;
  SigRange s = inputSigRange(instr);
  for (ArrayRange<Use> u = useRange(instr); !u.empty(); s.popFront()) {
    const Use& arg = u.popFront();
    const Type* sig_type = s.front();
    assert(subtypeof(type(arg), sig_type)); // ir must be well typed!
    if (isLinear(sig_type) || isState(sig_type))
      continue; // ignore stateful args
    if (argc < limit) {
      assert(argc < (int)MAXARGS && "too many args");
      args[argc++] = def_ins(arg);
    } else {
      emitStore(arg, sig_type, args_, offset, ACCSET_OTHER);
      offset += (model(sig_type) == kModelDouble) ? sizeof(double) : sizeof(Atom);
      varargc++;
    }
  }
  if (fixc != -1) {
    assert(argc + 2 < (int)MAXARGS && "too many args");
    args[argc++] = InsConst(varargc);
    args[argc++] = args_;
  }
  // reverse the args.
  assert(argc == call->count_args());
  for (uint32_t i = 0; i < argc/2; ++i) {
    LIns* t = args[i];
    args[i] = args[argc - i - 1];
    args[argc - i - 1] = t;
  }
  LIns* call_ins = lirout->insCall(call, args);
  if (k == HR_throw)
    frag->lastIns = call_ins;
  // assume the result goes in def 0 or def 1
  switch (numDefs(instr)) {
    default:
      assert(false && "too many defs");
    case 0: // do nothing
      break;
    case 1:
      set_def_ins(&getDefs(instr)[0], call_ins);
      break;
    case 2:
      set_def_ins(&getDefs(instr)[1], call_ins);
      break;
  }
  if (kind(instr) == HR_throw) {
    // generate an unreachable return to indicate block-end to Assembler.
    retp(InsConstPtr(0)); // never reached
  }
}

/**
 * Allocate memory to hold an argument list of default parameter values,
 * for use by generated code.  The layout is the same as if the values were
 * passed in as arguments, allowing generated code to load values like this:
 *   value = (argc < k ? defaults : args)[k]
 * Which can use a conditional move and no branching.  For example:
 *
 *   function f(arg1, arg2, optional1=1, optional2=2)
 *                       this, arg1, arg2, optional1, optional2
 * default-arg values:  [0,    0,    0,    1,         2]
 */
uint32_t* allocDefaultParams(CodeMgr* mgr, MethodSignaturep signature) {
  int required_count = signature->requiredParamCount();
  // iterate through params once to compute size.
  ParamIter sizer(signature);
  for (; !sizer.empty(); sizer.popFront()) {
  }
  // Allocate space for args, then iterate again to initialize memory.
  uint32_t* args = (uint32_t*) mgr->allocator.alloc(sizer.disp());
  ParamIter p(signature, args);
  for (int i = 0; !p.empty(); p.popFront(), ++i) {
    if (i <= required_count) {
      // required arg.  put a zero in the default arguments memory.
      switch (p.model()) {
        case kModelInt:
          p.asInt() = 0;
          break;
        case kModelDouble:
          p.asDouble() = 0;
          break;
        default:
          p.asAtom() = 0; // covers pointers, too.
          break;
      }
    } else {
      Atom value = signature->getDefaultValue(i - 1 - required_count);
      switch (avmplus::valueStorageType(p.builtinType())) {
        default: assert(false && "bad slotStorageType");
        case SST_int32:
          p.asInt() = AvmCore::integer_i(value);
          break;
        case SST_uint32:
          p.asUint() = AvmCore::integer_u(value);
          break;
        case SST_bool32:
          p.asUint() = atomGetBoolean(value) ? 1 : 0;
          break;
        case SST_double:
          p.asDouble() = AvmCore::number_d(value);
          break;
        case SST_scriptobject:
          p.asObject() = (ScriptObject*)atomPtr(value);
          break;
        case SST_string:
          p.asString() = (String*)atomPtr(value);
          break;
        case SST_namespace:
          p.asNamespace() = (Namespace*)atomPtr(value);
          break;
        case SST_atom:
          p.asAtom() = value;
          break;
      }
    }
  }
  return args;
}

/// Generate code for HR_start by loading each non-dead formal parameter
/// from the argument array.
///
void LirEmitter::do_start(StartInstr* start) {
  assert(!cxt->method->needArguments() && "arguments not supported yet");
  // + 3 because start has extra params not in included in param_count():
  // effect, env, and this.
  assert(numDefs(start) == signature->param_count() + 3 +
         (cxt->method->needRestOrArguments() ? 1 : 0));
  // Set up env parameter
  int i = 0;
  set_def_ins(start->data_param(i++), env_param);
  // Set up parameters.

  int required_count = signature->requiredParamCount();
  ParamIter p(signature);
  LIns* default_args = 0;
  if (signature->optional_count() > 0) {
    uint32_t* args = allocDefaultParams(pool->codeMgr, signature);
    default_args = InsConstPtr(args);
  }
  for (; !p.empty(); p.popFront(), ++i) {
    Def* arg_def = start->data_param(i);
    if (!arg_def->isUsed())
      continue;
    int offset = p.disp();
    LIns* args = ap_param;
    int arg_index = i - 2; // don't count env or this.
    if (arg_index >= required_count) {
      // args = (argc <= arg_index ? default_args : ap_param)
      LIns* cond = lirout->ins2(LIR_lei, argc_param, InsConst(arg_index));
      args = lirout->insChoose(cond, default_args, ap_param, true);
    }
    // arg = load [args + offset], according to arg type.
    switch (lirtype(type(arg_def))) {
      default: assert(false && "bad lirtype");
      case kLirInt:
        // int/uint/bool are widened to [u]intptr_t, so truncate now.  Loading
        // and truncating handles both endian styles.
        set_def_ins(arg_def, p2i(ldp(args, offset, ACCSET_OTHER, LOAD_CONST)));
        break;
      case kLirDouble:
        set_def_ins(arg_def, ldd(args, offset, ACCSET_OTHER, LOAD_CONST));
        break;
      case kLirPtr:
        set_def_ins(arg_def, ldp(args, offset, ACCSET_OTHER, LOAD_CONST));
        break;
    }
  }
  if (cxt->method->needRest())
    set_def_ins(start->data_param(i), callIns(&ci_createRestHelper, 3,
                                              env_param, argc_param, ap_param));
}

void LirEmitter::do_const(ConstantExpr* instr) {
  Def* def = instr->value();
  set_def_ins(def, emitConst(type(def)));
}

// Outline of CodegenLIR OP_callmethod
//
// verify that args_ are already coerced (debug only)
// try to inlineBuiltinFunction() (non-debugger only)
//   isNaN
//   charCodeAt
//   charAt
// emitCall
//   loadVTable(obj, objType) might call toVTable()
//   load env pointer from vtable
//   insAlloc(space for outgoing args_) // align, too
//   for each arg:
//     load from vars[]
//     store to arg space
//   load impl ptr from env
//   callIns [maybe iid signature]
//   LIR_livep(args_)

/// TODO use resolved ENV if available
///
void LirEmitter::do_loadenv(BinaryExpr* instr) {
  int disp_id = ordinalVal(type(instr->lhs_in()));
  const Use& object = instr->rhs_in();
  LIns* callee_env = emitLoadMethodEnv(emitLoadVTable(object), disp_id);
  set_def_ins(instr->value_out(), callee_env);
}

/// TODO use resolved ENV if available
///
void LirEmitter::do_loadinitenv(UnaryExpr* instr) {
  const Use& object = instr->value_in();
  LIns* callee_env = ldp(emitLoadVTable(object), offsetof(VTable, init),
                         ACCSET_OTHER, LOAD_CONST);
  set_def_ins(instr->value_out(), callee_env);
}

/// TODO use resolved ENV if available
///
void LirEmitter::do_loadsuperinitenv(UnaryExpr* instr) {
  LIns* env_in = def_ins(instr->value_in());
  LIns* scope = ldp(env_in, JitFriend::env_scope_offset, ACCSET_OTHER,
                    LOAD_CONST);
  LIns* vtable = ldp(scope, JitFriend::scope_vtable_offset, ACCSET_OTHER,
                     LOAD_CONST);
  LIns* base = ldp(vtable, offsetof(VTable, base), ACCSET_OTHER, LOAD_CONST);
  LIns* env = ldp(base, offsetof(VTable, init), ACCSET_OTHER, LOAD_CONST);
  set_def_ins(instr->value_out(), env);
}

/// TODO use resolved stuff if available
///
void LirEmitter::do_callmethod(CallStmt2* call) {
  const Use& env_in = call->param_in();
  MethodInfo* callee = getMethod(type(env_in));
  MethodSignaturep callee_sig = callee->getMethodSignature();
  LIns* callee_env = def_ins(env_in);
  LIns* result = emitAvmCall(call->args(), call->arg_count(), 
                             callee_sig, callee_env, call->value_out());
  set_def_ins(call->value_out(), result);
}

void LirEmitter::do_callinterface(CallStmt2* call) {
  MethodInfo* interface_info = halfmoon::getMethod(type(call->param_in())); 
  MethodSignaturep interface_sig = interface_info->getMethodSignature();
  LIns* callee_env = def_ins(call->param_in());
  LIns* iid = InsConstPtr((void*)JitFriend::getIID(interface_info));

  Def* value_out = call->value_out();
  const Type* value_out_type = type(value_out);
  assert(isBottom(value_out_type) ||
         model(value_out_type) == defaultModelKind(interface_sig->returnTraits()));

  emitStoreArgs(call->args(), call->arg_count(), interface_sig);
  LIns* result = emitInterfaceAvmCall(value_out_type, callee_env, 
                                      call->arg_count(), args_, iid);
  set_def_ins(call->value_out(), result);
}

void LirEmitter::do_callstatic(CallStmt2* call) {
  const Use& method_in = call->param_in();
  MethodInfo* callee = getMethod(type(method_in));
  MethodSignaturep callee_sig = callee->getMethodSignature();
  LIns* callee_method = def_ins(method_in);
  LIns* result = emitAvmCall(call->args(), call->arg_count(), 
                             callee_sig, callee_method, call->value_out());
  set_def_ins(call->value_out(), result);
}

void LirEmitter::do_return(StopInstr* stop) {
  if (have_catchblocks_) {
    // _ef.endTry();
    callIns(FUNCTIONID(hmEndTry), 1, _ef);
  }

  // brute force; tear down MethodFrame
  popMethodFrame(method_frame_);
  frag->lastIns = emitReturn(stop->value_in());
}

/// This goto is a fall-through path if it jumps to the next block in linear order.
///
bool LirEmitter::isFallthruGoto(GotoInstr* go) {
  assert(blocks_[current_block_] == ir->blockStart(go) &&
         "fallthru check on non-current block");
  int i = current_block_ + 1;
  return i < num_blocks_ && blocks_[i] == go->target;
}

/// This label is a fall-through path if the only goto is the previous block
/// in linear order.
bool LirEmitter::isFallthruLabel(LabelInstr* label) {
  assert(blocks_[current_block_] == label &&
         "fallthru check on non-current block");
  PredRange p(label);
  if (p.empty())
    return false; // 0 predecessors
  GotoInstr* go = p.popFront();
  if (!p.empty())
    return false; // 2+ predecessors
  return current_block_ > 0 &&
         blocks_[current_block_ - 1] == ir->blockStart(go);
}

bool LirEmitter::enableSSE() {
#if defined AVMPLUS_AMD64
  return true;
#elif defined AVMPLUS_IA32
  return core->config.njconfig.i386_sse2 != 0;
#else
  return false;
#endif
}


// Save our current PC location for the catch finder later.
void LirEmitter::emitSetPc(DeoptSafepointInstr* instr)
{
  int vpc = instr->vpc;
  // update bytecode ip if necessary
  stp(InsConstPtr((void*)vpc), _save_eip, 0, ACCSET_OTHER);
}


/// BRANCH PATCHING
///
/// label/goto: do_goto() and do_label() take care of patching.  Each goto
/// (except fall-thrus) generate a LIR_j (jump) and saves it; if do_label() has
/// come first, the jump is already patched.  Otherwise: do_label() generates
/// a LIR_label, and patches any previously generated jumps.
///
/// arm/if: do_if() and do_arm() take care of all patching, since Arms
/// only have one instruction to patch.  If do_if() comes first, we emit
/// the jump instruction, save it, then patch it in do_arm().  If do_arm() is
/// first, emit the LIR_label, save it, then in do_if() use it when generating
/// the branch instruction.
///
/// switch/block: do_switch() emits a LIR_jtbl instruction with one entry for
/// each Arm, plus one LIR_jt for the default case.  The LIns* for LIR_jtbl is
/// saved on each Def.  Patching proceeds the same was as for do_if(), but
/// without support for fall-through paths.
///

/// The branch to this Arm is a fall-through iff the Arm's block is the next
/// block in linear order *and* the owner supports the fall-through
/// optimization.  (HR_switch does not).
///
bool LirEmitter::isFallthruBranch(ArmInstr* arm) {
  assert(blocks_[current_block_] == ir->blockStart(arm->owner) &&
         "fallthru check on non-current block");
  int i = current_block_ + 1;
  return i < num_blocks_ && blocks_[i] == arm && (kind(arm->owner) == HR_if);
}

/// This Arm is a fall-through target iff its owner is the
/// previous block in linear order *and* the owner supports the fall-through
/// optimization.  (HR_switch does not).
///
bool LirEmitter::isFallthruArm(ArmInstr* arm) {
  assert((blocks_[current_block_] == arm) &&
         "fallthru check on non-current block");
  CondInstr* owner = arm->owner;
  BlockStartInstr* owner_block = InstrGraph::blockStart(owner);
  return kind(owner) == HR_if &&
         current_block_ > 0 && blocks_[current_block_ - 1] == owner_block;
}

void LirEmitter::do_goto(GotoInstr* go) {
  // for each paramter at the target, store the value for this edge.
  for (int i = 0, n = numUses(go); i < n; ++i) {
    const Use& arg = go->args[i];
    if (!isLinear(type(arg)) && !isState(type(arg))) {
      emitStore(arg, type(go->target->params[i]), args_,
                i << VARSHIFT(cxt->method),
                ACCSET_OTHER);
    }
  }
  if (!isFallthruGoto(go)) {
    LIns* label = ins(go->target);
    set_ins(go, emitJump(label));
    emitLiveHints(go->target);
  }
}

void LirEmitter::do_label(LabelInstr* instr) {
  // Label could be a fall-through if it only has one predecessor.
  if (!isFallthruLabel(instr)) {
    // generate the label and patch any existing goto's
    LIns* label_ins = set_ins(instr, emitLabel(instr));
    for (PredRange p(instr); !p.empty(); p.popFront()) {
      LIns* jump = ins(p.front());
      if (jump)
        jump->setTarget(label_ins);
    }
  }

  if (have_loop_)
    emitStopFence(instr);

  // load stuff passed by incoming gotos
  for (int i = 0, n = instr->paramc; i < n; ++i) {
    Def* param = &instr->params[i];
    const Type* param_type = type(param);
    if (!isLinear(param_type) && !isState(param_type)) {
      set_def_ins(
          param,
          emitLoad(param_type, args_, i << VARSHIFT(cxt->method),
                   ACCSET_OTHER, LOAD_NORMAL));
    }
  }
}

void LirEmitter::emitBeginCatch() {
  if (emittedBeginCatch)
    return;

  if (have_catchblocks_) {
    emittedBeginCatch = true;

    // exception case
    LIns* catch_label = setName(label(), "catch");

    catch_branch->setTarget(catch_label);
    
    // This regfence is necessary for correctness,
    // as register contents after a longjmp are unpredictable.
    lirout->ins0(LIR_regfence);
    
    MethodInfo* info = cxt->method;

    // _ef.beginCatch()
    int stackBase = signature->stack_base();
    LIns* pc = lirout->insLoad(LIR_ldp, _save_eip, 0, ACCSET_OTHER, LOAD_NORMAL);
    LIns* slotAddr = lea(stackBase << VARSHIFT(info) , safepoint_space_);
    LIns* handler_ordinal = callIns(FUNCTIONID(hmBeginCatch), 5, coreAddr, _ef, InsConstPtr(info), pc, slotAddr);

    (void)handler_ordinal;

    int handler_count = info->abc_exceptions()->exception_count;
    // Jump to catch handler
    // Find last handler, to optimize branches generated below.
    int i;
    for (i = handler_count-1; i >= 0; i--) {
      ExceptionHandler* h = &info->abc_exceptions()->exceptions[i];
      CatchBlock* cb = catchLabels->get(h->target);
      if (cb != NULL) break;
    }
    int last_ordinal = i;
    // There should be at least one reachable handler.
    AvmAssert(last_ordinal >= 0);
    // Do a compare & branch to each possible target.
    for (int j = 0; j <= last_ordinal; j++) {
      ExceptionHandler* h = &info->abc_exceptions()->exceptions[j];
      CatchBlock* cb = catchLabels->get(h->target);
      if (cb != NULL) {
        if (j == last_ordinal) {
          cb->jmp = lirout->insBranch(LIR_j, NULL, NULL);
        } else {
          LIns* cond = binaryIns(LIR_eqi, handler_ordinal, InsConst(j));
          cb->jmp = lirout->insBranch(LIR_jt, cond, NULL);
        }
      }
    }

    livep(_ef);
    livep(_save_eip);
    livep(safepoint_space_);
  }


}


void LirEmitter::do_catchblock(CatchBlockInstr* instr) {
  emitBeginCatch();

  emitCatchLabel(instr);

  lirout->ins0(LIR_regfence);

  int pc = instr->vpc;
  stp(InsConstPtr((void*)pc), _save_eip, 0, ACCSET_OTHER);
  lastPcSave = pc;

  // load stuff passed by incoming gotos
  for (int i = 0, n = instr->paramc; i < n; ++i) {
    Def* param = &instr->params[i];
    if (!param->isUsed())
      continue;
    const Type* param_type = type(param);
    if (!isLinear(param_type) && !isState(param_type)) {
      set_def_ins(
          param,
          emitLoad(param_type, safepoint_space_, i << VARSHIFT(cxt->method),
                   ACCSET_OTHER, LOAD_NORMAL));
    }
  }
}

/// Emit a conditional branch.  We optimize fall-through paths; if the taken
/// arm is the next block, then swap arms and reverse the sense
/// of the test.  Then, if the not-taken arm is the next block, omit the jump.
///
/// Patching is done here and in do_arm(), depending on which is seen first for
/// each arm.  See the "BRANCH PATCHING" comment.
///
void LirEmitter::do_if(IfInstr* instr) {
  ArmInstr *taken_arm, *fallthru_arm;
  LOpcode branch_opcode;

  // fixme: if any arm has come before the branch, *and* if argc > 0, then
  // we must emit stores here, and loads at the position of the ARM, because
  // LIR requres defs before uses in linear order.  For arguments to arms
  // that come later, we can set the LIns* for each arm parameter to the
  // LIns* for each arg.

  if (isFallthruBranch(instr->true_arm())) {
    // True edge is the fall-through path; reverse the sense of the branch.
    branch_opcode = LIR_jf;
    taken_arm = instr->false_arm();
    fallthru_arm = instr->true_arm();
  } else {
    // Normal sense of branch.
    branch_opcode = LIR_jt;
    taken_arm = instr->true_arm();
    fallthru_arm = instr->false_arm();
  }

  Instr* cond_instr = definer(instr->cond());
  LIns* cond_ins = def_ins(instr->cond());
  if (!cond_ins->isCmp() || ir->findBlockEnd(cond_instr) != instr) {
    // Generate a comparison in this block, which achevies two goals:
    // 1. The input to a LIR_jt or jf is always a comparison
    // 2. the comparison is always computed in this block.  Assemblers will
    //    sink comparisons to the point of a branch, which extends the life
    //    of the comparison's inputs.  Our lifetime analysis cannot predict
    //    this and doesn't take the sinking into account.  Generating a compare
    //    here avoids the problem.
    branch_opcode = invertCondJmpOpcode(branch_opcode);
    cond_ins = eqi0(cond_ins);
  }

  // If we have encountered either Arm already, get its LIR_label.  the label
  // will be NULL if we haven't seen the arm yet.
  LIns* branch_target = ins(taken_arm);
  LIns* jump_target = ins(fallthru_arm);

  // Emit the branch instructions.
  LIns* branch = lirout->insBranch(branch_opcode, cond_ins, branch_target);
  LIns* jump = isFallthruBranch(fallthru_arm) ? 0 : emitJump(jump_target);

  // Now save the branch instructions, in case the Arm comes later.
  set_ins(taken_arm, branch);
  set_ins(fallthru_arm, jump);
}

void LirEmitter::emitStopFence(BlockStartInstr* block) {
  if (shape(InstrGraph::blockEnd(block)) == STOPINSTR_SHAPE) {
    // if this block exits, and the function has a loop, then put a
    // regfence before the exit to prevent callee-saved register assignments
    // increasing register pressure in the loop.
    lirout->ins0(LIR_regfence);
  }
}

/// Emit a LIR_label, if we need one, and take care of patching.  See the
/// comment on patchBranches() for details.
///
void LirEmitter::do_arm(ArmInstr* arm) {
  if (isFallthruArm(arm)) {
    // This edge is a fall-through, no label is needed.
    return;
  }

  // get the branch to patch - if the arm's owner has already
  // been visited, this won't be null
  LIns* branch = ins(arm);

  // Generate the LIR_label, and save it in case this Arm 
  // is visited before its owner.
  LIns* label = set_ins(arm, emitLabel(arm));

  if (have_loop_)
    emitStopFence(arm);

  // Patch the branch, if the branch came first.
  if (branch) {
    if (branch->isop(LIR_jtbl)) {
      assert(kind(arm->owner) == HR_switch);
      branch->setTarget(pos(arm), label);
      // If incoming edge is from a switch, we need a regfence.
      // See case LIR_jtbl in Assembler::gen() in nanojit/Assembler.cpp.
      lirout->ins0(LIR_regfence);
    } else {
      branch->setTarget(label);
    }
  }
}

void LirEmitter::do_switch(SwitchInstr* instr) {
  int num_cases = instr->num_cases();
  const Use& selector_in = instr->selector();
  LIns* selector = def_ins(selector_in); // default arm

  // If we have encountered either Arm already, get its LIR_label.  the label
  // will be NULL if we haven't seen the arm yet.
  ArmInstr* default_arm = instr->default_arm();
  LIns* default_target = ins(default_arm);
  // if (unsigned(selector) >= numCases) goto default
  LIns* default_ins = jgeui(selector, num_cases, default_target);
  // Now save the branch instructions, in case the Arm comes later.
  set_ins(default_arm, default_ins);

  // case arms
  LIns* switch_ins = lirout->insJtbl(selector, num_cases);
  for (int i = 0; i < num_cases; ++i) {
    ArmInstr* case_arm = instr->case_arm(i);
    LIns* case_target = ins(case_arm);
    if (case_target)
      switch_ins->setTarget(i, case_target);
    else
      set_ins(case_arm, switch_ins);
  }
}

void LirEmitter::do_greaterthan(BinaryExpr* instr) {
  LIns* compare_atom = emitCompare(def_ins(instr->rhs_in()),
                                   def_ins(instr->lhs_in()));
  LIns* bool_out = eqp(compare_atom, InsConstAtom(trueAtom));
  set_def_ins(instr->value_out(), bool_out);
}

void LirEmitter::do_lessthan(BinaryExpr* instr) {
  LIns* compare_atom = emitCompare(def_ins(instr->lhs_in()),
                                   def_ins(instr->rhs_in()));
  LIns* bool_out = eqp(compare_atom, InsConstAtom(trueAtom));
  set_def_ins(instr->value_out(), bool_out);
}

void LirEmitter::do_cast(BinaryStmt* instr) {
  Def* value_in = def(instr->rhs_in());
  LIns* value_ins = def_ins(value_in);
  LIns* result_ins = downcast_obj(value_ins, env_param,
                                  traitsVal(type(instr->lhs_in())));
  set_def_ins(instr->value_out(), result_ins);
}

void LirEmitter::do_modulo(BinaryExpr* instr) {
  LIns* value_out = callIns(FUNCTIONID(mod), 2, def_ins(instr->lhs_in()), 
                def_ins(instr->rhs_in()));
  set_def_ins(instr->value_out(), value_out);
}

void LirEmitter::doBinaryInstr(BinaryExpr* instr, LOpcode lir_opcode) {
  LIns* value_out = lirout->ins2(lir_opcode, def_ins(instr->lhs_in()),
                                 def_ins(instr->rhs_in()));
  set_def_ins(instr->value_out(), value_out);
}

void LirEmitter::addSpeculativeTypeCheck(const Use& value_in, Atom value_type) {
  LIns* get_type_tag = andp(def_ins(value_in), kAtomTypeMask);
  LIns* isType = eqp(get_type_tag, value_type);
  LIns* typecheck_fail = lirout->insBranch(LIR_jf, isType, 0);
  bailout_branches_.add(typecheck_fail);
}

/// Speculate and always upcoerce an int/number into the number
/// type. This helps because atomToDouble boxes doubles that can fit into an int
/// as the int type. If we speculated number, we'd deopt when its maybe better to
/// just upconvert to a number. boids_untyped is slower for now
void LirEmitter::do_speculate_numeric(BinaryExpr* instr) {
  const Use& value_in = instr->lhs_in();
  LIns* get_type_tag = andp(def_ins(value_in), kAtomTypeMask);

  LIns* isInt = eqp(get_type_tag, kIntptrType);
  LIns* isNumber = eqp(get_type_tag, kDoubleType);

  LIns* isNumeric = eqi0(lirout->ins2(LIR_ori, isInt, isNumber));
  LIns* typecheck_fail = lirout->insBranch(LIR_jt, isNumeric, 0);

  bailout_branches_.add(typecheck_fail);
  set_def_ins(instr->value_out(), callIns(FUNCTIONID(number), 
              1, def_ins(instr->lhs_in())));
}

void LirEmitter::do_speculate_int(BinaryExpr* instr) {
  addSpeculativeTypeCheck(instr->lhs_in(), kIntptrType);
  set_def_ins(instr->value_out(), callIns(FUNCTIONID(integer_i), 
              1, def_ins(instr->lhs_in())));
}

void LirEmitter::do_speculate_number(BinaryExpr* instr) {
  addSpeculativeTypeCheck(instr->lhs_in(), kDoubleType);
  set_def_ins(instr->value_out(), callIns(FUNCTIONID(number), 
              1, def_ins(instr->lhs_in())));
}

void LirEmitter::do_speculate_string(BinaryExpr* instr) {
  addSpeculativeTypeCheck(instr->lhs_in(), kStringType);
  set_def_ins(instr->value_out(), callIns(FUNCTIONID(string), 2, coreAddr,
                                          def_ins(instr->lhs_in())));
}

void LirEmitter::do_speculate_object(BinaryExpr* instr) {
  addSpeculativeTypeCheck(instr->lhs_in(), kObjectType);
  set_def_ins(instr->value_out(), def_ins(instr->lhs_in()));
}

void LirEmitter::do_speculate_bool(BinaryExpr* instr) {
  addSpeculativeTypeCheck(instr->lhs_in(), kBooleanType);
  LIns* bool_val = eqp(def_ins(instr->lhs_in()), trueAtom);
  set_def_ins(instr->value_out(), bool_val);
}

/***
 * Check of incoming is an object, else bailout
 * then try to convert to array, else bailout
 */
void LirEmitter::do_speculate_array(BinaryExpr* instr) {
  addSpeculativeTypeCheck(instr->lhs_in(), kObjectType);
  LIns* script_object = atomToScriptObject(def_ins(instr->lhs_in()));
  // toArrayObject() returns NULL if not an array object
  LIns* array_object = callIns(FUNCTIONID(scriptObjectToArrayObject),
                               1, script_object);
  LIns* not_array = eqp(array_object, (Atom) NULL);
  LIns* typecheck_fail = lirout->insBranch(LIR_jt, not_array, 0);
  bailout_branches_.add(typecheck_fail);

  set_def_ins(instr->value_out(), array_object);
}

void LirEmitter::doUnaryInstr(UnaryExpr* instr, LOpcode lir_opcode) {
  LIns* value_out = lirout->ins1(lir_opcode, def_ins(instr->value_in()));
  set_def_ins(instr->value_out(), value_out);
}

LIns* LirEmitter::atomToScriptObject(LIns* object) {
  return andp(object, ~7);
}

void LirEmitter::writeSlot(LIns* object, LIns* value_in, 
                          const Type* value_type, uint32_t offset) {
  switch (model(value_type)) {
    default:
      printf("model is: %d\n", model(value_type));
      assert(false && "Unsupported model");
    case kModelAtom: {
      callIns(&ci_atomWriteBarrier, 4, InsConstPtr(core->gc), object, 
              addp(object, offset), value_in);
      break;
    }
    case kModelScriptObject:
    case kModelString:
    case kModelNamespace:
      callIns(&ci_privateWriteBarrierRC, 4, InsConstPtr(core->gc), object,
              addp(object, offset), value_in);
      break;
    case kModelInt:
      sti(value_in, object, offset, ACCSET_OTHER);
      break;
    case kModelDouble:
      std(value_in, object, offset, ACCSET_OTHER);
      break;
  }
}

void LirEmitter::do_setslot(CallStmt2* instr) {
  const Use& name_in = instr->param_in();
  const Use& object_in = instr->object_in();
  const Use& value_in = instr->vararg(0);

  int slot = ordinalVal(type(name_in));
  const Type* value_type = type(value_in);
  const Type* object_type = type(object_in);
  assert(subtypeof(value_type, ir->lattice.getSlotType(object_type, slot)));
  uint32_t offset = ir->lattice.getSlotOffset(object_type, slot);

  LIns* object = def_ins(object_in);
  writeSlot(object, def_ins(value_in), value_type, offset);
}

void LirEmitter::do_getslot(CallStmt2* instr) {
  const Use& name_in = instr->param_in();
  const Use& object_in = instr->object_in();

  int slot = ordinalVal(type(name_in));
  const Type* object_type = type(object_in);
  const Type* slot_type = ir->lattice.getSlotType(object_type, slot);
  assert(subtypeof(type(instr->value_out()), slot_type));
  uint32_t offset = ir->lattice.getSlotOffset(object_type, slot);

  LIns* object = def_ins(object_in);
  LIns* value = emitLoad(slot_type, object, offset, ACCSET_OTHER, LOAD_NORMAL);
  set_def_ins(instr->value_out(), value);
}

void LirEmitter::do_newinstance(UnaryExpr* instr) {
  // copied from CodegenLIR::emitCall() case OP_construct
  const Use& ctor_in = instr->value_in();
  LIns* vtable = emitLoadVTable(ctor_in);
  LIns* ivtable = ldp(vtable, offsetof(VTable, ivtable), ACCSET_OTHER,
                      LOAD_CONST);
  LIns* funcptr = ldp(ivtable, offsetof(VTable, createInstanceProc),
                      ACCSET_OTHER);
  LIns* object = callIns(&ci_createInstanceProc, 2, funcptr, def_ins(ctor_in));
  set_def_ins(instr->value_out(), object);
}

// i2u is just a copy at the machine level, so point to the input LIns*.
void LirEmitter::do_i2u(UnaryExpr* instr) {
  set_def_ins(instr->value_out(), def_ins(instr->value_in()));
}

// u2i is just a copy at the machine level, so point to the input LIns*.
void LirEmitter::do_u2i(UnaryExpr* instr) {
  set_def_ins(instr->value_out(), def_ins(instr->value_in()));
}

/**
 * Generate a handler for a function that takes (MethodEnv*) args.  The label
 * is created on demand on the first call, and just returned for subsequent
 * calls.
 */
LIns* LirEmitter::emitHandler(LIns** label, const CallInfo* call) {
  if (!*label) {
    LIns* args[] = { env_param };
    *label = traps_lir->ins0(LIR_label);
    traps_lir->insCall(call, args);
  }
  return *label;
}

/** Generate one handler for all kConvertNullToObjectError errors. */
LIns* LirEmitter::emitNpeHandler() {
  return emitHandler(&npe_label, &ci_npe);
}

/** Generate one handler for all kConvertUndefinedToObjectError errors. */
LIns* LirEmitter::emitUpeHandler() {
  return emitHandler(&upe_label, &ci_upe);
}

/** Generate one handler for all timeout interrupts */
LIns* LirEmitter::emitInterruptHandler() {
  if (interrupt_label == NULL) {
    LIns* args[] = { env_param };
    interrupt_label = traps_lir->ins0(LIR_label);
    
#ifdef VMCFG_INTERRUPT_SAFEPOINT_POLL
    traps_lir->ins0(LIR_pushstate);
#endif 
    traps_lir->ins0(LIR_regfence);
    traps_lir->insCall(&ci_handleInterruptMethodEnv, args);
#ifdef VMCFG_INTERRUPT_SAFEPOINT_POLL
    traps_lir->ins0(LIR_popstate);
    traps_lir->ins0(LIR_restorepc);
#endif
    
  }
  return interrupt_label;
}

void LirEmitter::do_cknull(UnaryStmt* instr) {
  const Use& value_in = instr->value_in();
  LIns* ptr = def_ins(value_in);
  LIns* undefined_ins = InsConstAtom(undefinedAtom);
  lirout->insBranch(LIR_jt, ltup(ptr, undefined_ins), emitNpeHandler());
  lirout->insBranch(LIR_jt, eqp(ptr, undefined_ins), emitUpeHandler());
  set_def_ins(instr->value_out(), ptr);
}

void LirEmitter::do_cknullobject(UnaryStmt* instr) {
  const Use& value_in = instr->value_in();
  LIns* ptr = def_ins(value_in);
  lirout->insBranch(LIR_jt, eqp0(ptr), emitNpeHandler());
  set_def_ins(instr->value_out(), ptr);
}

void LirEmitter::do_cktimeout(UnaryStmt* instr) {
  // Omit timeout checks if they are turned off.  We don't do this further
  // upstream, because it can cause the graph to have no endpoints, if there
  // is an infinite loop.
#ifdef VMCFG_INTERRUPT_SAFEPOINT_POLL
  bool check_interrupt = true;
#else
  bool check_interrupt = core->config.interrupts;
#endif
  if (check_interrupt) {
    lirout->ins0(LIR_savepc);
    LIns* interrupted = ldi(coreAddr, JitFriend::core_interrupted_offset,
                            ACCSET_OTHER, LOAD_VOLATILE);
    LIns* cond = eqi(interrupted, AvmCore::NotInterrupted);
    lirout->insBranch(LIR_jf, cond, emitInterruptHandler());
    lirout->ins0(LIR_discardpc);
  }
  set_def_ins(instr->value_out(), InsConst(0)); // always return false.
}

void LirEmitter::do_doubletoint32(UnaryExpr* instr) {
  LIns* arg = def_ins(instr->value_in());

  LIns* result;
  if (enableSSE()) {
    // For SSE capable machines, inline our double to integer conversion
    // using the CVTTSD2SI instruction.  If we get a 0x80000000 return
    // value, our double is outside the valid integer range we fallback
    // to calling doubleToInt32.
    //
    //  result = t = d2i(arg)
    //  if (t == 0x80000000)
    //    result = doubleToInt32(arg)
    LIns* intResult = lirout->insAlloc(sizeof(int32_t));
    LIns* fastd2i = lirout->ins1(LIR_d2i, arg);
    sti(fastd2i, intResult, 0, ACCSET_STORE_ANY);      // int32_t index
    LIns *c = lirout->ins2(LIR_eqi, fastd2i, InsConst(1L << 31));
    LIns* br = lirout->insBranch(LIR_jf, c, 0);
    // slow path
    LIns *funcCall = callIns(&ci_doubleToInt32, 1, arg);
    sti(funcCall, intResult, 0, ACCSET_STORE_ANY);      // int32_t index
    LIns* label = lirout->ins0(LIR_label);
    br->setTarget(label);
    result = ldi(intResult, 0, ACCSET_LOAD_ANY);
  } else {
    result = callIns(&ci_doubleToInt32, 1, arg);
  }

  set_def_ins(instr->value_out(), result);
}

void LirEmitter::do_abc_getprop(CallStmt2* instr) {
  const Multiname* name = nameVal(type(instr->param_in()));
  GetCache* cache = get_cache_builder.allocateCacheSlot(name);
  LIns* cache_addr = InsConstPtr(cache);
  LIns* value = callIns(
      &ci_get_cache_handler, 4,
      ldp(cache_addr, offsetof(GetCache, get_handler), ACCSET_OTHER),
      cache_addr, env_param, def_ins(instr->object_in()));
  set_def_ins(instr->value_out(), value);
}

void LirEmitter::do_abc_setprop(CallStmt2* instr) {
  const Use& value = instr->vararg(0);
  LIns* value_ins = def_ins(value);
  const Multiname* name = nameVal(type(instr->param_in()));

  SetCache* cache = set_cache_builder.allocateCacheSlot(name);
  LIns* cache_addr = InsConstPtr(cache);
  callIns(&ci_set_cache_handler, 5,
          ldp(cache_addr, offsetof(SetCache, set_handler), ACCSET_OTHER),
          cache_addr, def_ins(instr->object_in()),
          value_ins, env_param);
}

void LirEmitter::do_abc_callprop(CallStmt2* instr) {
  const Multiname* name = nameVal(type(instr->param_in()));
  CallCache* cache = call_cache_builder.allocateCacheSlot(name);
  int arg_count = instr->arg_count();

  emitAtomArgs(instr->args(), arg_count);
  LIns* cache_addr = InsConstPtr(cache);
  LIns* value = callIns(
      &ci_call_cache_handler, 6,
      ldp(cache_addr, offsetof(CallCache, call_handler), ACCSET_OTHER),
      cache_addr, def_ins(instr->object_in()), InsConst(arg_count - 1), args_,
      env_param);
  set_def_ins(instr->value_out(), value);
}

void LirEmitter::do_abc_add(BinaryStmt* instr) {
  const Use& lhs = instr->lhs_in();
  const Use& rhs = instr->rhs_in();

  LIns* left_operand = def_ins(lhs);
  LIns* right_operand = def_ins(rhs);
  LIns* result = callIns(&ci_op_add_a_aa, 3, 
                        coreAddr, left_operand, right_operand);
  set_def_ins(instr->value_out(), result);
}

/// changes model int to model atom
void LirEmitter::do_int2atom(UnaryExpr* i) {
  emitHelperCall2(i, &ci_intToAtom);
}

/// changes model double to atom
void LirEmitter::do_double2atom(UnaryExpr* i) {
  emitHelperCall2(i, &ci_doubleToAtom);
}

/// tags an atom value with the given tag
void LirEmitter::doTagPointer(UnaryExpr* instr, Atom tag) {
  const Use& value_in = instr->value_in();
  LIns* result = addp(def_ins(value_in), tag);
  set_def_ins(instr->value_out(), result);
}

/// changes model atom to scriptobject
void LirEmitter::do_atom2scriptobject(UnaryExpr* instr) {
  const Use& value_in = instr->value_in();
  LIns* result = andp(def_ins(value_in), kAtomPtrMask);
  set_def_ins(instr->value_out(), result);
}

/// changes model bool to atom
void LirEmitter::do_bool2atom(UnaryExpr* instr) {
  Def* value_in = def(instr->value_in());
  LIns* result = nativeToAtom(def_ins(value_in), getTraits(type(value_in)));
  set_def_ins(instr->value_out(), result);
}

void LirEmitter::do_getouterscope(BinaryExpr* instr) {
  int scope_index = ordinalVal(type(instr->lhs_in()));
  LIns* env_in = def_ins(instr->rhs_in()); // env to use
  LIns* scope_chain = ldp(env_in, JitFriend::env_scope_offset, ACCSET_OTHER,
                          LOAD_CONST);
  LIns* value = ldp(scope_chain,
                    JitFriend::scope_scopes_offset + scope_index * sizeof(Atom),
                    ACCSET_OTHER, LOAD_CONST);
  set_def_ins(instr->value_out(), value);
}

void LirEmitter::do_loadenv_interface(BinaryExpr* instr) {
  const Use& object_in = instr->rhs_in();
  MethodInfo *info = methodVal(type(instr->lhs_in()));
  int imt_index = JitFriend::hashIID(info);
  LIns* vtable = emitLoadVTable(object_in);
  LIns* env = ldp(vtable,
                  JitFriend::vtable_imt_offset + imt_index * sizeof(ImtThunkEnv*),
                  ACCSET_OTHER, LOAD_CONST);
  set_def_ins(instr->value_out(), env);
}

void LirEmitter::do_not(UnaryExpr* instr) {
  LIns* x = def_ins(instr->value_in());
  set_def_ins(instr->value_out(), eqi0(x));
}

void LirEmitter::emitHelperCall2(UnaryExpr* instr, const CallInfo* call) {
  const Use& value_in = instr->value_in();
  LIns* result = callIns(call, 2, coreAddr, def_ins(value_in));
  set_def_ins(instr->value_out(), result);
}

/// Newstate allocates space on the stack for safepoints
/// Currently assumes newstate occurs in the entry block prior to any safepoint 
/// or setlocal instructions.
void LirEmitter::do_newstate(ConstantExpr* instr) {
  if (safepoint_space_ != NULL) {
    set_def_ins(instr->value(), safepoint_space_);
  }
}

/// Saves the ABC local var, which also represents ABC operand stack values
/// Stores the local in its native format + type tag. Hopefully
/// NanoJIT removes the tag stores
void LirEmitter::do_setlocal(SetlocalInstr* instr) {
  set_def_ins(instr->state_out(), def_ins(instr->state_in()));
  assert (safepoint_space_ != NULL);
  int stackIndex = instr->index;
  emitStore(instr->value_in(), type(instr->value_in()),
            safepoint_space_, stackIndex << VARSHIFT(cxt->method),
            ACCSET_STORE_ANY);
  set_def_ins(instr->state_out(), def_ins(instr->state_in()));

  /// This is really ugly, to directly access J2.
  /// Still contemplating if we should make
  /// deoptGenerator a class or something and make it
  /// walk through the halfmoon IR. Seems overkill for now. Defer
  /// but don't let this stay - Mason
  /// can't put in abcbuilder because type analysis may specialize downstream
  JitManager* jit = JitManager::init(this->pool);
  BailoutData* metaData = jit->ensureMethodData(cxt->method)->bailout_data;
  assert (metaData != NULL);
  metaData->setNativeType(stackIndex, type2sst(type(instr->value_in())));
}

/// Generates LIR for a safepoint instruction
/// Any safepoint instr that gets here cannot be optimized away
/// Stores the abc pc, setlocals store the actual data prior to this safepoint
void LirEmitter::do_safepoint(SafepointInstr* instr) {
  assert (safepoint_space_ != NULL);
  int vpc = instr->vpc;
  // update bytecode ip
  stp(InsConstPtr((void*)vpc), _save_eip, 0, ACCSET_OTHER);
  
  /// See do_setlocal for why we grab metadata here
  JitManager* jit = JitManager::init(this->pool);
  BailoutData* metaData = jit->ensureMethodData(cxt->method)->bailout_data;
  assert (metaData != NULL);
  metaData->do_safepoint(vpc, instr->scopep, instr->sp);
}

// DEOPT
void LirEmitter::do_deopt_safepoint(DeoptSafepointInstr* instr) {
  lirout->insSafe(LIR_safe, (void*)instr);
}

// DEOPT
void LirEmitter::do_deopt_finish(DeoptFinishInstr* instr) {
  lirout->insSafe(LIR_endsafe, (void*)instr->safepoint);
}

// DEOPT
void LirEmitter::do_deopt_finishcall(DeoptFinishCallInstr* instr) {
  instr->safepoint->rtype = type2sst(type(instr->value_in()));
  lirout->insSafe(LIR_endsafe, (void*)instr->safepoint);
}

// has next 2 modifies the object and index in place.
void LirEmitter::do_abc_hasnext2(Hasnext2Stmt* instr) {
  LIns* object = stackAlloc(sizeof(Atom), "hasnext2_obj");
  LIns* counter = stackAlloc(sizeof(int32_t), "hasnext2_index");

  assert (model(type(instr->object_in())) == kModelAtom);
  assert (model(type(instr->counter_in())) == kModelInt);

  stp(def_ins(instr->object_in()), object, 0, ACCSET_OTHER);
  sti(def_ins(instr->counter_in()), counter, 0, ACCSET_OTHER);
  LIns* new_index = callIns(&ci_hasnextproto, 3, env_param, object, counter);

  set_def_ins(instr->value_out(), new_index);
  set_def_ins(instr->object_out(), 
    emitLoad(type(instr->object_in()), object, 0, ACCSET_OTHER, LOAD_NORMAL));

  set_def_ins(instr->counter_out(),
    emitLoad(type(instr->counter_in()), counter, 0, ACCSET_OTHER, LOAD_NORMAL));
}

} // end LirEmitter

#include "generated/Stub_lirtable.hh"

#endif // VMCFG_HALFMOON
