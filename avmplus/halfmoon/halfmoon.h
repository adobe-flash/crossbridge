/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace halfmoon {
using avmplus::AbcEnv;
using avmplus::CodegenDriver;
using avmplus::CodeWriter;
using avmplus::FrameState;
using avmplus::MethodInfo;
using avmplus::MethodSignaturep;
using avmplus::Toplevel;
using avmplus::Traits;
using avmplus::GprMethodProc;
using avmplus::PrintWriter;
using nanojit::Allocator;
using nanojit::Fragment;
using nanojit::HashMap;

/// Return true if halfmoon wants to try to compile this method.
///
bool canCompile(MethodInfo*);
bool isProfilerEnabled();

class JitManager;
class AbcGraph;
class AbcBlock;
class Lattice;
class InstrGraph; //TODO: matz_inline_experiment
class ProfiledInformation;
class BailoutData;
class ScopeInfo;
struct Context; //TODO: matz_inline_experiment

/// I wish I could think of a better name here.  JitWriter is the CodeWriter
/// interface to the Halfmoon JIT.
///
class JitWriter: public CodeWriter {
public:
  /** construct a JitWriter for a plain compilation session */
  JitWriter(MethodInfo*, Toplevel* topLevel, AbcEnv *abc_env);

  /** Construct a JitWriter for inlining */
  JitWriter(MethodInfo*, const Context *calling_context);
  virtual ~JitWriter();

  /// Return the implementation or NULL if we didn't generate one.
  ///
  GprMethodProc finish();

  /// Return the compiled IR, or null if it failed for some reason.
  ///
  InstrGraph* ir();

  // CodeWriter methods
  void writePrologue(const FrameState* state, const uint8_t *pc, CodegenDriver*);
  void write(const FrameState* state, const uint8_t* pc, AbcOpcode opcode, Traits *type);
  void writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type);
  void writeOp2(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, uint32_t opd2, Traits* type);
  void writeMethodCall(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, MethodInfo*, uintptr_t disp_id, uint32_t argc, Traits* type);
  void writeNip(const FrameState* state, const uint8_t *pc, uint32_t count);
  void writeCheckNull(const FrameState* state, uint32_t index);
  void writeCoerce(const FrameState* state, uint32_t index, Traits *type);
  void writeEpilogue(const FrameState* state);
  void writeBlockStart(const FrameState* state);

private:
  void analyze(AbcOpcode abcop, const uint8_t* pc, const FrameState*);
  void finishBlock(const uint8_t* nextpc);
  void newBlock(const uint8_t* pc, const FrameState* state);
  void startBlock(const FrameState*);

public:
  InstrGraph *ir_;  //TODO: matz_inline_experiment review this comment. (stash graph for inlined callees here)
  //maybe put callers context here? Then compile callee could set it when compiling for inliner. matz
private:
  Allocator alloc_;
  MethodInfo* method_;
  PrintWriter& console_;
  JitManager* jit_mgr_;
  AbcGraph* abc_;
  AbcBlock* current_block_;
  AbcEnv *abc_env_;    //TODO: matz_inline_experiment review this comment. (stash this aside so inliner will have it)
  Toplevel *toplevel_; //TODO: matz_inline_experiment review this comment.
  ProfiledInformation* profiled_info_;
  const Context *calling_context_;
};

/// Validate that LIR is well formed.
/// 1. Every branch target must have a non-null LIns*.
/// 2. Each def must dominate each use (SSA).
/// 3. Each def must come before each use in linear order (Assembler)
/// 4. Any def before a loop and used in the loop needs a live range that
///    spans the whole loop.
///
/// For the purposes of this analyzer, A loop is formed by an edge from B2->B1
/// whenever B2 comes after B1 in linear order, ignoring control-flow paths.
/// An instruction is "in the loop" if its between B1.first_ins and B2.last_ins.
///
/// Returns true on success.  Failures print a diagnostic and assert (debug),
/// or return false (release).
///
using avmplus::AvmLogControl;
bool checkLir(Fragment*, AvmLogControl* logc);

} // namespace halfmoon
