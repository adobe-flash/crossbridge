/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HALFMOON_ABCBUILDER_H_
#define HALFMOON_ABCBUILDER_H_

namespace halfmoon {
using nanojit::BitSet;
using avmplus::ExceptionHandler;
using profiler::RecordedType;

/**
 * An AbcBuilder creates a instr graph by walking the blocks of ABC in a
 * function body, simulating each instruction and peephole-optimizing
 * as the graph is built.
 */
class AbcBuilder {
  enum NameArity {
    kNameKnown, // name with no args, e.g. o.name
    kNameIndex, // name with index on stack, e.g. o[index]
    kNameNs, // name with namespace on stack, e.g. o.ns::name
    kNameNsIndex
  // name with namespace and index on stack o.ns::[index]
  };

public:
  AbcBuilder(MethodInfo*, AbcGraph* abc, InstrFactory*, Toplevel*, ProfiledInformation* profiled_info, bool has_reachable_exceptions);
  ~AbcBuilder();
  InstrGraph* visitBlocks(Seq<AbcBlock*>*);

private:
  class InitVisitor: public avmplus::InitVisitor {
  public:
    InitVisitor(AbcBuilder* abc, Def* object) : abc(abc), object(object) {}
    virtual ~InitVisitor() {}
    void defaultVal(Atom val, uint32_t slot, Traits* slot_type);
  private:
    AbcBuilder* abc;
    Def* object;
  };
  void speculateParameters(StartInstr* start);
  void buildStart();
  void emitInitializers(Def* object);
  void finish();
  bool checkFrame(Def* frame[], int sp, int scopep);

  AbcOpcode readInstr(const uint8_t* &pc, const uint8_t* end, uint32_t &imm30,
                      uint32_t& imm30b, int &imm24, int &imm8);
  void visitBlock(AbcBlock*);
  void safepointStmt(const uint8_t* pc);
  DeoptSafepointInstr* emitBailoutSafepoint(const uint8_t* pc); // DEOPT
  DeoptSafepointInstr* emitThrowSafepoint(const uint8_t* pc); // DEOPT
  DeoptSafepointInstr* emitCallSafepoint(const uint8_t* pc, int nargs); // DEOPT
  void emitFinish(DeoptSafepointInstr* sfp); // DEOPT
  void emitFinishCall(DeoptSafepointInstr* sfp, Def* arg);  // DEOPT
  void printAbcInstr(const uint8_t* pc);
  Def* peephole(Def*);
  void set_effect(Def*); // Update current effect.
  void set_state(Def*); // Update current state.
  void visitInstr(const uint8_t* pc, AbcOpcode, uint32_t imm30, uint32_t imm30b, int32_t imm8);
  Traits* getNamedTraits(uint32_t index);
  NameArity nameArity(const Type* name);
  NameArity nameArity(Def* name) {
    return nameArity(type(name));
  }
  Def* constName(uint32_t index);
  void pushConst(const Type* type);
  void pushDef(Def*);
  Def* finishStmt(Instr* instr, Def* effect, Def* value);
  Def* naryStmt1(InstrKind, Def* name, Def** args, int argc);
  Def* naryStmt2(InstrKind, Def* name, Def* obj, Def** args, int extra_argc);
  Def* naryStmt3(InstrKind, Def* name, Def* obj, Def* index, Def** args,
                 int extra_argc);
  Def* naryStmt4(InstrKind, Def* name, Def* obj, Def* ns, Def* index,
                 Def** args, int extra_argc);
  // Build an NaryStmt4 with an extra fixed arg
  Def* naryStmt4(InstrKind, Def* name, Def* obj, Def* ns, Def* index, Def* index2,
                 Def** args, int extra_argc);
  Def* callStmt2(InstrKind, Def* name, Def* obj, Def** args, int extra_argc);
  Def* callStmt3(InstrKind, Def* name, Def* index, Def* obj, Def** args,
                 int extra_argc);
  Def* callStmt4(InstrKind, Def* name, Def* ns, Def* index, Def* obj,
                 Def** args, int extra_argc);
  Def* callStmt(const InstrKind*, uint32_t name_index, int argc);
  Def* initStmt(uint32_t name_index);
  Def* newclassStmt(int scope_count, Def* base, Def* scopes[],
                    uint32_t class_id);
  Def* findStmt(const InstrKind*, uint32_t name_index, AbcInstr*);
  Def* getlexStmt(uint32_t name_index, AbcInstr*);
  Def* finddefStmt(Def* name);
  Def* naryStmt(InstrKind, int argc);
  Def* binaryStmt(InstrKind, Def* arg0, Def* arg1);
  Def* binaryExpr(InstrKind, Def* arg0, Def* arg1);
  Def* unaryStmt(InstrKind, Def* val_in);
  Def* unaryExpr(InstrKind, Def* value);
  void constructsuperStmt(int argc);
  Def* coerceArgs(Def* obj, Def** args, int argc, MethodInfo* m);
  void stopStmt(InstrKind, Def*, LabelInstr**);
  Def* nullCheck(Def* ptr);
  void debugInstr(InstrKind, Def* value);

  void ifStmt(bool sense, Def* cond);
  void addIf(bool sense, Def* cond);
  void setFrameArgs(BlockEndInstr* end);
  void addArm(int i, ArmInstr* arm, bool switch_arm = false);
  void switchStmt(uint32_t num_cases, Def* index);
  void addSwitch(uint32_t num_cases, Def* index);
  void addGoto(AbcBlock* to);
  LabelInstr* ensureBlockLabel(AbcBlock* abc_block);
  CatchBlockInstr* ensureCatchBlockLabel(AbcBlock* abc_block);
  void linkExceptionEdge(BlockStartInstr* block, CatchBlockInstr* catch_block);

  void cktimeout();
  void jumpStmt();
  Def* newcatchStmt(uint32_t imm30);
  void setslotStmt(uint32_t slot, Def* obj, Def* val);
  Def* getslotStmt(Def* obj, uint32_t slot);
  Def* getouterscopeStmt(uint32_t scope_index);
  Def* getglobalscope();
  Def* hasnext2Stmt(Def** obj, Def** ctr);
  void startBlock(AbcBlock*);
  void printStartState(AbcBlock*);
  void printFrameState();
  Def** popArgs(int argc);
  Def* peekDef();
  Def* popDef();
  Def* coerceExpr(Traits*, Def*);

  Def* toNumber(Def* val);
  Def* toInt(Def* val);

  void saveState(const uint8_t* pc, int scopep_offset, int stackp_offset); 
  bool hasType(Def* val, RecordedType abc_type);
  InstrKind getSpeculativeKind(RecordedType recorded_type);
  Def* toSpeculativeType(Def* val, RecordedType abc_type);
  bool isType(Def* val, RecordedType abc_type);

  void createSetLocalInstr(int i, Def* val);
  void setLocal(int i, Def* val);
  bool needAbcState();

  bool handlerCoversPc(const uint8_t* pc);
  bool pcIsHandler(const uint8_t* pc);

  // Profiler methods
  bool shouldSpeculate();
  Def* getTypedDef(const uint8_t* pc, Def* current_def, int input_index, int input_count, int output_count);
  RecordedType getRecordedType(const uint8_t* pc, int input_index, int input_count, int output_count);
  Def* typeSpecializedBinary(AbcOpcode abcop, const uint8_t* pc, Def* lhs, Def* rhs);

private:
  Def* createConst(const Type* type) {
    return builder_.addConst(type);
  }
  Def* ordinalConst(int x) {
    return createConst(lattice_.makeOrdinalConst(x));
  }
  Def* traitsConst(Traits* t) {
    return createConst(lattice_.makeTraitsConst(t));
  }
  Def* traitsConst(uint32_t index) {
    return traitsConst(getNamedTraits(index));
  }
  Def* getLocal(int i) {
    return frame_[i];
  }
  template<class E> FrameRange<E> frameRange(E* frame) {
    return FrameRange < E > (frame, stackp_, scopep_, stack_base_);
  }
  void returnStmt(Def* value) {
    stopStmt(HR_return, value, &return_label_);
  }
  void throwStmt(Def* value) {
    stopStmt(HR_throw, value, &throw_label_);
  }
  int scope_count() const {
    return scopep_ + 1 - scope_base_;
  }
  Def** scopes() const {
    return &frame_[scope_base_];
  }
  Def* &state() {
    return frame_[state_pos_];
  }
  Def* effect() {
    return frame_[effect_pos_];
  }

  /// Get this function's env parameter.
  ///
  Def* env_param() {
    return cast<StartInstr>(ir_->begin)->data_param(0);
  }

private:
  Allocator& alloc_;
  Allocator0 alloc0_;
  MethodInfo* const method_;
  MethodSignaturep const sig_;
  AbcGraph* const abc_;
  PoolObject* const pool_;
  PrintWriter& console_;
  Lattice& lattice_;
  const int scope_base_;
  const int stack_base_;
  const int framesize_;
  const int num_vars_; // framesize + effect + state
  const int effect_pos_; // position of effect within vars[] array
  const int state_pos_; // position of state within vars[] array
  const int setlocal_pos_;

  int scopep_;
  int stackp_;
  int withbase_;
  Def** const frame_; // most recent data results
  AbcBlock* abc_block_; // current block

  const uint8_t * const code_pos_;
  const uint8_t * pc_;
  bool has_reachable_exceptions_;
  LabelInstr* return_label_;
  LabelInstr* throw_label_;
  InstrFactory& factory_;
  InstrKind kind_map_[256]; // AbcOpcode -> InstrKind, indexed by AbcOpcode.
  InstrGraph* const ir_;
  InstrGraphBuilder builder_;
  Def* never_def_;
  ProfiledInformation* profiled_info_;
  Toplevel* toplevel_;
};

} // namespace avmplus

#endif // HALFMOON_ABCBUILDER_H_
