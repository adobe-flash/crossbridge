/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace halfmoon {
using avmplus::CacheBuilder;
using avmplus::CallCache;
using avmplus::GetCache;
using avmplus::LirHelper;
using avmplus::SetCache;
using namespace nanojit;

/// LirHelper2 holds methods we'd like to put in LirHelper.  They are
/// here to make merging easier.
///
class LirHelper2: public LirHelper {
public:

  LirHelper2(PoolObject* pool)
  : LirHelper(pool)
  , verbose_writer(0)
  {
  }

  LirBuffer* createLirout(bool verbose, const char* title);
  LIns* setName(LIns* ins, const char* name);
  LIns* initMethodFrame(LIns* env_param);
  void popMethodFrame(LIns* method_frame);
  CodeList* assemble(const nanojit::Config& config,
                     HMDeoptDataWriter* mdwriter);
  LIns* stackAlloc(int32_t num_bytes, const char* name) {
    return setName(lirout->insAlloc(num_bytes), name);
  }
  LIns* param(int n, const char* name = 0) {
    return setName(lirout->insParam(n, 0), name);
  }
  LIns* ldp(LIns* ptr, int32_t d, AccSet acc_set, LoadQual load_qual = LOAD_NORMAL) {
    return lirout->insLoad(LIR_ldp, ptr, d, acc_set, load_qual);
  }
  LIns* ldd(LIns* ptr, int32_t d, AccSet acc_set, LoadQual load_qual = LOAD_NORMAL) {
    return lirout->insLoad(LIR_ldd, ptr, d, acc_set, load_qual);
  }
  LIns* ldi(LIns* ptr, int32_t d, AccSet acc_set, LoadQual load_qual = LOAD_NORMAL) {
    return lirout->insLoad(LIR_ldi, ptr, d, acc_set, load_qual);
  }
  LIns* jgeui(LIns* a, uint32_t b, LIns* target) {
    return lirout->insBranch(LIR_jt, lirout->ins2(LIR_geui, a, InsConst(b)), target);
  }
  LIns* retd(LIns* value) {
    return lirout->ins1(LIR_retd, value);
  }
  LIns* eqi(LIns* a, int b) {
    return lirout->ins2(LIR_eqi, a, InsConst(b));
  }

public:
#ifdef NJ_VERBOSE
  VerboseWriter* verbose_writer;
#else
  void* verbose_writer;
#endif
};

/// LirEmitter generates LIR intermediate code for one InstrGraph, by:
/// 1. Sort blocks into a nanojit-friendly order: Defs appear before Uses in
///    linear order.  We arbitrarily use reverse postorder over successors
///    starting from ir->begin.
/// 2. Find live ranges that require LIR_live hints.
/// 3. Generate LIR by visiting each instruction in each block, forwards.
///
/// LirEmitter extends LirHelper so it can easily use various helper
/// functions also shared by CodegenLIR and InvokerCompiler.
///
class LirEmitter: public KindAdapter<LirEmitter, void>, private LirHelper2 {
public:
  LirEmitter(Context* cxt, InstrGraph* ir, ProfiledInformation* profiled_info);
  ~LirEmitter();

public:
  /// Emit all the LIR.
  ///
  GprMethodProc emit(DeoptData**);

public:
  /// Emit LIR for one instruction.
  ///
  void emit(Instr*);

  /// Finish up by patching branches, then generating machine code.
  ///
  GprMethodProc finish(DeoptData**);

public: // ADAPTER impl
  void do_default(Instr*);
  void do_start(StartInstr*);
  void do_const(ConstantExpr*);
  // fixme: re-implement-this using finddef_cache void do_abc_finddef(BinaryStmt*);
  void do_loadenv(BinaryExpr*);
  void do_loadinitenv(UnaryExpr*);
  void do_loadsuperinitenv(UnaryExpr*);
  void do_loadenv_interface(BinaryExpr*);
  void do_callmethod(CallStmt2*);
  void do_callinterface(CallStmt2*);
  void do_callstatic(CallStmt2*);
  void do_newinstance(UnaryExpr*);
  void do_return(StopInstr*);
  void do_arm(ArmInstr*);
  void do_label(LabelInstr*);
  void do_goto(GotoInstr*);
  void do_if(IfInstr*);
  void do_catchblock(CatchBlockInstr*);
  void do_setslot(CallStmt2*);
  void do_abc_setprop(CallStmt2*);
  void do_abc_callprop(CallStmt2*);
  void do_addd(BinaryExpr* i) { doBinaryInstr(i, LIR_addd); }
  void do_subd(BinaryExpr* i) { doBinaryInstr(i, LIR_subd); }
  void do_muld(BinaryExpr* i) { doBinaryInstr(i, LIR_muld); }
  void do_modulo(BinaryExpr*);
  void do_divd(BinaryExpr* i) { doBinaryInstr(i, LIR_divd); }
  void do_lessthan(BinaryExpr*);
  void do_greaterthan(BinaryExpr*);
  void do_abc_add(BinaryStmt*);
  void do_addi(BinaryExpr* i) { doBinaryInstr(i, LIR_addi); }
  void do_subi(BinaryExpr* i) { doBinaryInstr(i, LIR_subi); }
  void do_muli(BinaryExpr* i) { doBinaryInstr(i, LIR_muli); }
  void do_andi(BinaryExpr* i) { doBinaryInstr(i, LIR_andi); }
  void do_ori(BinaryExpr* i)  { doBinaryInstr(i, LIR_ori); }
  void do_xori(BinaryExpr* i) { doBinaryInstr(i, LIR_xori); }
  void do_lshi(BinaryExpr* i) { doBinaryInstr(i, LIR_lshi); }
  void do_rshi(BinaryExpr* i) { doBinaryInstr(i, LIR_rshi); }
  void do_rshui(BinaryExpr* i) { doBinaryInstr(i, LIR_rshui); }
  void do_cast(BinaryStmt*);
  void do_abc_getprop(CallStmt2*);
  void do_getslot(CallStmt2*);
  void do_negd(UnaryExpr* i) { doUnaryInstr(i, LIR_negd); }
  void do_noti(UnaryExpr* i) { doUnaryInstr(i, LIR_noti); }
  void do_not(UnaryExpr*);
  void do_cknull(UnaryStmt*);
  void do_cknullobject(UnaryStmt*);
  void do_switch(SwitchInstr*);
  void do_eqi(BinaryExpr* i) { doBinaryInstr(i, LIR_eqi); }
  void do_lti(BinaryExpr* i) { doBinaryInstr(i, LIR_lti); }
  void do_lei(BinaryExpr* i) { doBinaryInstr(i, LIR_lei); }
  void do_gti(BinaryExpr* i) { doBinaryInstr(i, LIR_gti); }
  void do_gei(BinaryExpr* i) { doBinaryInstr(i, LIR_gei); }
  void do_eqd(BinaryExpr* i) { doBinaryInstr(i, LIR_eqd); }
  void do_ltd(BinaryExpr* i) { doBinaryInstr(i, LIR_ltd); }
  void do_led(BinaryExpr* i) { doBinaryInstr(i, LIR_led); }
  void do_gtd(BinaryExpr* i) { doBinaryInstr(i, LIR_gtd); }
  void do_ged(BinaryExpr* i) { doBinaryInstr(i, LIR_ged); }
  void do_equi(BinaryExpr* i) { do_eqi(i); }
  void do_ltui(BinaryExpr* i) { doBinaryInstr(i, LIR_ltui); }
  void do_leui(BinaryExpr* i) { doBinaryInstr(i, LIR_leui); }
  void do_gtui(BinaryExpr* i) { doBinaryInstr(i, LIR_gtui); }
  void do_geui(BinaryExpr* i) { doBinaryInstr(i, LIR_geui); }
  void do_eqb(BinaryExpr* i) { doBinaryInstr(i, LIR_eqi); }
  void do_eqp(BinaryExpr* i) { doBinaryInstr(i, LIR_eqp); }
  void do_i2d(UnaryExpr* i) { doUnaryInstr(i, LIR_i2d); }
  void do_d2i(UnaryExpr* i) { doUnaryInstr(i, LIR_d2i); }
  void do_doubletoint32(UnaryExpr*);
  void do_i2u(UnaryExpr*);
  void do_u2i(UnaryExpr*);
  void do_u2d(UnaryExpr* i) { doUnaryInstr(i, LIR_ui2d); }
  void do_speculate_numeric(BinaryExpr*);
  void do_speculate_number(BinaryExpr*);
  void do_speculate_int(BinaryExpr*);
  void do_speculate_string(BinaryExpr*);
  void do_speculate_object(BinaryExpr*);
  void do_speculate_array(BinaryExpr*);
  void do_speculate_bool(BinaryExpr*);
  void do_newstate(ConstantExpr*);
  void do_setlocal(SetlocalInstr*);
  void do_safepoint(SafepointInstr*);
  void do_deopt_safepoint(DeoptSafepointInstr* instr); // DEOPT
  void do_deopt_finish(DeoptFinishInstr* instr); // DEOPT
  void do_deopt_finishcall(DeoptFinishCallInstr* instr); // DEOPT
  void do_int2atom(UnaryExpr*);
  void do_double2atom(UnaryExpr*);
  void do_atom2scriptobject(UnaryExpr*);
  void do_scriptobject2atom(UnaryExpr* i) { doTagPointer(i, kObjectType); }
  void do_string2atom(UnaryExpr* i) { doTagPointer(i, kStringType); }
  void do_ns2atom(UnaryExpr* i) { doTagPointer(i, kNamespaceType); }
  void do_bool2atom(UnaryExpr* i);
  void do_getouterscope(BinaryExpr* i);
  void do_abc_hasnext2(Hasnext2Stmt*);
  void do_cktimeout(UnaryStmt*);

private:
  /// Generate simple LIR for a binary instruction that maps directly
  /// to a single LIR binary instruction.
  void doBinaryInstr(BinaryExpr*, LOpcode);
  void doModuloInstr(BinaryExpr*);
  void doUnaryInstr(UnaryExpr*, LOpcode);
  LIns* atomToScriptObject(LIns* object);
  void speculate_numeric(BinaryExpr*);
  void addSpeculativeTypeCheck(const Use& value_in, Atom value_type);
  void do_stub(Instr*, const CallInfo*);
  void doTagPointer(UnaryExpr* instr, Atom tag);
  void writeSlot(LIns* object, LIns* value, const Type* value_type, uint32_t offset);

  // helpers to emit canned hunks of LIR
  LIns* emitLoadVTable(const Use& object);
  LIns* emitLoadMethodEnv(LIns* vtable, int disp_id);
  void emitStoreArgs(const Use* args, int argc, MethodSignaturep callee);
  void emitAtomArgs(const Use* args, int argc);
  void copyBranchArgs();
  LIns* emitAvmCall(const Use* args, int argc, MethodSignaturep callee_sig,
                    LIns* callee_env, Def* value_out);
  LIns* emitIndirectAvmCall(const Type* result_type, LIns* callee_env, int argc,
                            LIns* args);
  LIns* emitInterfaceAvmCall(const Type* result_type, LIns* callee_env, int argc,
                            LIns* args, LIns* iid);
  LIns* emitConst(const Type* t);
  LIns* emitCompare(LIns* lhs_in, LIns* rhs_in);
  LIns* emitJump(LIns* target = 0);
  LIns* emitLoad(const Type* value_type, LIns* ptr, int32_t offset, AccSet,
                 LoadQual);
  LIns* emitStore(const Use& value, const Type* constraint, LIns* ptr,
                  int32_t offset, AccSet);
  LIns* emitReturn(const Use& value);
  LIns* emitNpeHandler();
  LIns* emitUpeHandler();
  LIns* emitInterruptHandler();
  LIns* emitHandler(LIns** label, const CallInfo* call);
  LIns* emitLabel(BlockStartInstr*);
  LIns* emitCatchLabel(CatchBlockInstr*);
  void emitHelperCall2(UnaryExpr*, const CallInfo* call);
  void emitLiveHints(Instr* target);
  void emitLive(Def*);
  void emitStackOverflowCheck();
  void emitBegin(bool has_reachable_exceptions);
  void emitInitializers(Def* object);
  void emitStopFence(BlockStartInstr*);

private:
  // other helpers
  void allocateTables();
  void printInstr(Instr*);
  void sortBlocks();
  void analyzeLiveness();
  void patchBailouts();
  bool isFallthruGoto(GotoInstr*);
  bool isFallthruBranch(ArmInstr*);
  bool isFallthruArm(ArmInstr*);
  bool isFallthruLabel(LabelInstr*);
  bool enableSSE();
  void emitSetPc(DeoptSafepointInstr* instr);

private:
  // helpers to access LIns* associated with each Def*
  LIns* set_def_ins(Def* d, LIns* ins);
  LIns* set_ins(Instr*, LIns* ins);
  LIns* def_ins(const Def*);
  LIns* def_ins(const Use&);
  LIns* ins(Instr*);

private:
  typedef HashMap<Instr*, Seq<Instr*>*> LoopLiveMap;

private:
  Context* cxt;
  MethodSignaturep signature; // Signature of method being compiled.
  InstrGraph* ir;
  LIns** def_ins_; // Array of LIR instructions, one per def.
  LIns** instr_ins; // Array of LIR instructions, one per instr
  LIns *env_param, *argc_param, *ap_param;
  LIns *method_frame_;
  LIns* args_; // one LIR_alloca with space for call and goto args.
  BlockStartInstr** blocks_; // Array of blocks in sorted order.
  int num_blocks_;
  int current_block_; // the blocks_[] index of the current block.
  int max_argc_;
  LoopLiveMap loop_live_; // List of live Instrs for each loop header block.
  LirWriter* traps_lir; // Writer for emitting trap handlers.
  LIns* traps_skip; // LIR_skip that links traps block to main code buffer.
  LIns* npe_label; // label to jump to for null pointer exceptions.
  LIns* upe_label; // label to jump to for undefined pointer exceptions.
  LIns* interrupt_label; // label for interrupt checks.
  LIns* safepoint_space_; // space to store safepoint data.
  CacheBuilder<GetCache> get_cache_builder;
  CacheBuilder<SetCache> set_cache_builder;
  CacheBuilder<CallCache> call_cache_builder;
  const bool enable_verbose_lir_;
  ProfiledInformation* profiled_info_;
  SeqBuilder<LIns*> bailout_branches_;
  bool have_safepoints;
  bool have_loop_;
  bool have_catchblocks_;
  bool emittedBeginCatch;

  LIns *_save_eip, *_ef;
  const uint8_t* code_pos;

  class CatchBlock {
   public:
    CatchBlockInstr* block;
    LIns* jmp;
    CatchBlock(CatchBlockInstr* b): block(b), jmp(NULL) {}
  };

  // This should really be int but gcc-4.2.1 on Mac sometimes screws
  // up lookups when it's int.
  HashMap<intptr_t, CatchBlock*> *catchLabels;

  LIns* catch_branch;
  int lastPcSave;
    
private:
  static bool haveStub(InstrKind);
  static const CallInfo lir_table[]; // callinfos for each stub
  static const int stub_fixc[];      // fixed arg count for each stub

  void emitBeginCatch();
  void emitBeginTry();
};

}
