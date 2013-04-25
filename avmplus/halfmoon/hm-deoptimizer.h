/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_DEOPT
#define HM_DEOPT

#include "../core/avmplus.h"

#ifdef VMCFG_HALFMOON
#include "../core/Deopt.h"     // DeoptContext

/// From the Google C++ Style Guide.
/// TODO: Move to a more generally-accessible location.

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

namespace halfmoon {

using namespace avmplus;
using namespace nanojit;

/// An DeoptData object represents the deoptimization metadata
/// for a single method, always manipulated via an DeoptData*
/// pointer.  The object as declared below is just the header for
/// a variable-length data structure represented as a chain of
/// memory chunks allocated by a MetaDataAlloc object. This gives
/// the header a "free ride" on the principal mechanism to used to
/// allocate data with the same lifetime as the method code, without
/// needing additional heap metadata overhead.

class DeoptData {
private:
  friend class HMDeoptDataWriter;
  friend class HMDeoptContext;

  const MethodSignature* signature_;

  // Instances are created only by HMDeoptDataWriter.
  DeoptData(const MethodSignature* sig)
    : signature_(sig)
  {}

  // This class is the header for a variable-length object.
  // The metadata instruction stream follows.  The instruction
  // stream is examined only by HMDeoptContext.
  NIns* instructions() {
    return (NIns*)((uint8_t*)this + sizeof(DeoptData));
  }

  // TODO: We'll probably want to stash a reference to the CodeMgr
  // here, or some way to find the MetaDataAlloc so we can free storage.
  // This can be worked out when we figure out what to do with the
  // code for obsolete translations.  At present, we deallocate both
  // code and metadata only upon JIT failure.
  
  // Instances of DeoptData may not be destroyed in the usual fashion.
  // Instead, use MetaDataAlloc::freeAll().
  ~DeoptData() {}
  DISALLOW_COPY_AND_ASSIGN(DeoptData);
public:
  Deoptimizer*  createDeoptimizer(MethodInfo* info);
};

enum SafepointKind {
  kInvalidSafepoint,
  kCallSafepoint,
  kInlineSafepoint,
  kThrowSafepoint,
  kBailoutSafepoint
};

/// Deoptimization metadata is composed of a sequence of 'instructions' of
/// bounded length.  The state description for a safepoint is determined by
/// interpreting these instructions, incrementally modifying the model until
/// the PC value of the model corresponds to that of the safepoint address.
///
/// TODO DEOPT
/// In practice, many operands of these instructions tend to be small, and we
/// can expect many operands to fit in units smaller than a byte.  A well-tuned
/// instruction encoding in this scheme would provide for some combination of
/// unaligned bit-fields of carefully selected sizes, fused instructions, long
/// and short forms of an instruction, and extension bytes.  We've sketched out
/// some preliminary ideas along these lines, but it's premature to settle on
/// such a scheme without experiments on a good-sized corpus of real examples.
///
/// At present, we implement a scheme with little optimization, but in which the
/// content of the instructions, and their bounded-length character, are shared
/// with what we expect of a fully fleshed out implementation.

enum MetaDataOpcode {
  // SET_NPC addr
  // Set the native pc to the specified value.
  MD_SET_NPC,

  // SET_SCOPE depth
  // Set the scope stack depth.
  MD_SET_SCOPE,

  // SET_STACK depth
  // Set the operand stack depth.
  MD_SET_STACK,

  // SLOT_INDEX slot index type
  // Set the definition of a canonical slot to a compiled frame index.
  MD_SLOT_IDX,

  // CALL rty vlen nargs
  // Mark the location of a call safepoint, giving the SST of the result
  // and the virtual pc increment across the call instruction.  These are
  // used only for calls to AS3 methods, not to helper functions.
  MD_CALL,

  // BAILOUT
  // Mark the location of a bailout safepoint.
  MD_BAILOUT,

  // THROW
  // Mark the location of a throw safepoint.
  // These are used only for helper call sites, not calls to AS3 methods.
  MD_THROW,

  // INLINE minfo
  // Mark the location of an inlined function, with its MethodInfo pointer.
  MD_INLINE,

  // END vpc_decrement npc_decrement
  // Mark the end of the most recent unclosed safepoint.
  MD_END,

  // LINK md_address
  // Chain to the next metadata chunk.
  MD_LINK
};

class HMDeoptDataWriter : public nanojit::MetaDataWriter {
public:

  HMDeoptDataWriter(CodeAlloc& md_alloc, Allocator& tmp_alloc,
                    const MethodSignature* signature, LIns** defs);

  // Callback entry points from assembler.
  void beginAssembly(Assembler* assm, uint8_t* address);
  void safepointStart(Assembler* assm, void* payload, uint8_t* address);
  void safepointEnd(Assembler* assm, void* payload, uint8_t* address);
  void setNativePc(uint8_t* address);
  void endAssembly(Assembler* assm, uint8_t* address);

  // Return pointer to DeoptData object.  Valid only after assembly is complete.
  DeoptData* finish();

  // If the assembly aborts, and we will abandoning the metadata, we need
  // to free up persistent storage allocated for the DeoptData.  The last
  // method invoked on an HMDeoptDataWriter prior to destruction should be
  // either finish() or abandon().
  void abandon();

private:
  DISALLOW_COPY_AND_ASSIGN(HMDeoptDataWriter);

  // Allocator for metadata, using discontiguous linked chunks like CodeAlloc.
  // The metadata stays writable at all times, however.
  CodeAlloc& md_alloc_;

  // Bump-pointer arena allocator for temporary storage during assembly.
  Allocator& tmp_alloc_;

  // Array of LIns* corresponding to definitions.
  LIns** def_ins_;

  // List of deopt metadata blocks so far generated.
  CodeList* data_;

  // Pointer to next byte to be written.
  // TODO: Fix abuse of NIns*.
  NIns* mdins_;

  // Pointers to start and end of the current chunk.
  // TODO: Fix abuse of NIns*.
  NIns* start_;
  NIns* end_;

  const MethodSignature* sig_;
  NIns* mdstart_;

  // Get the LIns* associated with a Def.
  // Cribbed from a private definition in LirEmitter.
  LIns* def_ins(const Def* d) {
    return def_ins_[defId(d)];
  }

  // Get the LIns* associated with a Use.
  // Cribbed from a private definition in LirEmitter.
  LIns* def_ins(const Use& u) {
    return def_ins(def(u));
  }

  // Static method info.
  int frame_size_;
  int stack_base_;
  int scope_base_;
  int n_locals_;
  
  // State model.
  uintptr_t npc_;
  uint32_t  vpc_;
  int scopep_;               // TODO: Make this unsigned.
  int stackp_;               // TODO: Make this unsigned.
  int* frame_slots_;
  uint8_t* frame_types_;

  void mdAlloc(NIns*& start, NIns*& end, NIns*& bytep);

  // Ensure that at least 'bytes' bytes are available in the
  // current code chunk.  If not, allocate a new chunk, emit
  // a link to it in the old chunk, and make the new chunk
  // current.
  void ensureSpace(int bytes);
  
  // Write unsigned integer values.
  void writeUInt8(uint8_t value);
  void writeUInt32(uint32_t value);
#ifdef VMCFG_64BIT
  void writeUInt64(uint64_t value);
#endif

  // Write an unsigned integer of sufficient size to hold a pointer value.
  void writeUIntPtr(uintptr_t value);

  // Write an unsigned integer encoded as 1-5 bytes in ULEB128 encoding.
  void writeULEB128(uint32_t value);

  // Return the number of byte needed to encode value in ULEB128.
  int byteCountULEB128(uint32_t value);

  // Write a signed integer encoded as 1-5 bytes in SLEB128 encoding.
  void writeSLEB128(int32_t value);

  // Return the number of byte needed to encode value in SLEB128.
  int byteCountSLEB128(int32_t value);

  void emitSetSlotIdx(DeoptSafepointInstr* safepoint,
                      Assembler* assm, int slot, int i);
};


/// Deoptimization context for compiled methods generated by Halfmoon.

class HMDeoptContext : /*implements*/ public DeoptContext {
private:
  DISALLOW_COPY_AND_ASSIGN(HMDeoptContext);

  friend class HMFramePopulator;
  friend class HMHandlerFramePopulator;
  friend class HMReturnFramePopulator;

  DeoptData* deopt_data_;
  const MethodSignature* signature_;

  int frame_size_;
  int stack_base_;
  int scope_base_;
  int n_locals_;

  SafepointKind kind_;
  uintptr_t npc_;
  uint32_t  vpc_;
  int scopep_;
  int stackp_;
  int* frame_slots_;
  uint8_t* frame_types_;
  int nargs_;
  int vlen_;
  SlotStorageType rtype_;
  MethodInfo* minfo_;

  NIns* mdins_;

  uint8_t readUInt8();
  uint32_t readUInt32();
#ifdef VMCFG_64BIT
  uint64_t readUInt64();
#endif
  uintptr_t readUIntPtr();
  uint32_t readULEB128();
  int32_t readSLEB128();

public:

  HMDeoptContext(DeoptData* data);
  ~HMDeoptContext();

  uint8_t* frameBase(MethodFrame* method_frame)
  {
    //...
    (void)method_frame;
    return 0;
  }

  MethodFrame* methodFrame(uint8_t* frame_base)
  {
    //...
    (void)frame_base;
    return NULL;
  }

  int32_t saveEip(uint8_t* frame_base)
  {
    //...
    (void)frame_base;
    return 0;
  }

  uint8_t* calleeFrameBase(uint8_t* frame_base)
  {
    //...
    (void)frame_base;
    return 0;
  }

  void setSafepointFromVirtualPc(int32_t vpc)
  {
    // NYI -- I don't think we'll need this.
    (void)vpc;
    AvmAssert(false);
  }

  void setSafepointFromNativePc(uint8_t* pc);

  int32_t safepointVpc()
  {
    return vpc_;
  }

  bool isSafepointAtThrow()
  {
    return (kind_ == kThrowSafepoint);
  }
  
  bool isSafepointAtAbcCall()
  {
    return (kind_ == kCallSafepoint);
  }

  SlotStorageType safepointReturnValueSST()
  {
    AvmAssert(kind_ == kCallSafepoint);
    return rtype_;
  }

  Atom interpretFromSafepoint(uint8_t* fp, MethodEnv* env) ;
  Atom interpretFromCallSafepoint(uint8_t* fp, MethodEnv* env, Atom return_val);
  Atom interpretFromThrowSafepoint(uint8_t* fp, MethodEnv* env, int32_t handler_vpc, Atom exn_val);
};

} // end namespace

#endif // VMCFG_HALFMOON
#endif // #ifndef HM_DEOPT
