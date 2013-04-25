/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#include "hm-deoptimizer.h"

#ifdef VMCFG_HALFMOON

namespace halfmoon {

using namespace avmplus;
using namespace nanojit;

void HMDeoptDataWriter::mdAlloc(NIns*& start, NIns*& end, NIns*& bytep)
{
  if (start)
    CodeAlloc::add(data_, start, end);

  // MetaDataAlloc contract: allocations never fail
  md_alloc_.alloc(start, end, 0);
  bytep = start;
}

HMDeoptDataWriter::HMDeoptDataWriter(CodeAlloc& md_alloc,
                                     Allocator& tmp_alloc,
                                     const MethodSignature* signature,
                                     LIns** defs)
  : md_alloc_(md_alloc),
    tmp_alloc_(tmp_alloc),
    def_ins_(defs),
    data_(NULL),
    mdins_(NULL),
    start_(NULL),
    end_(NULL),
    sig_(signature),
    mdstart_(NULL)
{
  mdAlloc(start_, end_, mdins_);

  mdstart_ = mdins_;

  // Leave space for HMDeoptData header.
  mdins_ += sizeof(DeoptData);

  frame_size_ = signature->frame_size();
  stack_base_ = signature->stack_base();
  scope_base_ = signature->scope_base();
  n_locals_   = signature->local_count();

  npc_ = 1;  // Must differ from sentinel value of 0.
  vpc_ = 0;
  scopep_ = 0;
  stackp_ = 0;
  frame_slots_ = new (tmp_alloc_) int[frame_size_];
  VMPI_memset(frame_slots_, 0, frame_size_ * sizeof(int));
  frame_types_ = new (tmp_alloc_) uint8_t[frame_size_];
  VMPI_memset(frame_types_, SST_MAX_VALUE, frame_size_ * sizeof(uint8_t));
}

//#define DEOPT_METADATA_DIAGNOSTIC_HACK

DeoptData* HMDeoptDataWriter::finish()
{
  AvmAssert(mdstart_ != NULL);
  md_alloc_.addRemainder(data_, start_, end_, start_, mdins_);

  // Placement new.  Create HMDeoptData header in space previously reserved.
  DeoptData* dd = new (mdstart_) DeoptData(sig_);

#ifdef DEOPT_METADATA_DIAGNOSTIC_HACK
  //fprintf(stderr, "READING\n");
  HMDeoptContext ctx(dd);
  // Force full parse of metadata.
  // Relies on sentinel set in endAssembly().
  ctx.setSafepointFromNativePc(0);
#endif

  return dd;
}

void HMDeoptDataWriter::abandon()
{
  md_alloc_.freeAll(data_);
  mdstart_ = NULL;
}

void HMDeoptDataWriter::ensureSpace(int bytes)
{
  // There had better be enough space for both the
  // instruction we wish to allocate, and the link
  // to the next chunk that may follow it.
  if (mdins_ + bytes + (1+sizeof(uintptr_t)) > end_) {
    mdAlloc(start_, end_, mdins_);
    writeUInt8(MD_LINK);
    writeUIntPtr(uintptr_t(mdins_));
  }
}
  
void HMDeoptDataWriter::writeUInt8(uint8_t value)
{
  *(mdins_++) = value;
}

void HMDeoptDataWriter::writeUInt32(uint32_t value)
{
  // Write bytewise to avoid unaligned writes.
  // Probably cheaper to write unaligned if the
  // machine handles it without a software trap.
  *(mdins_++) = uint8_t(value & 0x000000ff);
  *(mdins_++) = uint8_t((value & 0x0000ff00) >> 8);
  *(mdins_++) = uint8_t((value & 0x00ff0000) >> 16);
  *(mdins_++) = uint8_t((value & 0xff000000) >> 24);
}

#ifdef VMCFG_64BIT
void HMDeoptDataWriter::writeUInt64(uint64_t value)
{
  *(mdins_++) = uint8_t(value & 0x00000000000000ff);
  *(mdins_++) = uint8_t((value & 0x000000000000ff00) >> 8);
  *(mdins_++) = uint8_t((value & 0x0000000000ff0000) >> 16);
  *(mdins_++) = uint8_t((value & 0x00000000ff000000) >> 24);
  *(mdins_++) = uint8_t((value & 0x000000ff00000000) >> 32);
  *(mdins_++) = uint8_t((value & 0x0000ff0000000000) >> 40);
  *(mdins_++) = uint8_t((value & 0x00ff000000000000) >> 48);
  *(mdins_++) = uint8_t((value & 0xff00000000000000) >> 56);
}
#endif

void HMDeoptDataWriter::writeUIntPtr(uintptr_t value)
{
#ifdef VMCFG_64BIT
  writeUInt64(value);
#else
  writeUInt32(value);
#endif
}

void HMDeoptDataWriter::writeULEB128(uint32_t value)
{
  do {
    uint8_t byte = value & 0x7f;
    value >>= 7;
    if (value > 0) byte |= 0x80;
    *(mdins_++) = byte;
  } while (value > 0);
}

int HMDeoptDataWriter::byteCountULEB128(uint32_t value)
{
  int count = 0;
  do {
    count++;
    value >>= 7;
  } while (value > 0);
  return count;
}

void HMDeoptDataWriter::writeSLEB128(int32_t value)
{
  // TODO: Portability issue -- Signed >> must be an arithmetic shift.
  AvmAssert(-1 >> 1 == -1);
  for(;;) {
    uint8_t byte = value & 0x7f;
    value >>= 7;
    // Stop writing when the remaining bits are just copies of the sign bit.
    if (((value == 0) && !(byte & 0x40)) || ((value == -1) && (byte & 0x40))) {
      *(mdins_++) = byte;
      return;
    } else {
      *(mdins_++) = byte | 0x80;
    }
  }
}

int HMDeoptDataWriter::byteCountSLEB128(int32_t value)
{
  int count = 0;
  for(;;) {
    count++;
    int byte = value & 0x7f;
    value >>= 7;
    if (((value == 0) && !(byte & 0x40)) || ((value == -1) && (byte & 0x40))) {
      return count;
    }
  }
}

void HMDeoptDataWriter::beginAssembly(Assembler* assm, uint8_t* address)
{
  (void)assm;
  npc_ = uintptr_t(address);
  ensureSpace(1+sizeof(uintptr_t));
  writeUInt8(MD_SET_NPC);
  writeUIntPtr(uintptr_t(npc_));
}

void HMDeoptDataWriter::safepointStart(Assembler* assm, void* payload, uint8_t* address)
{
  (void)assm;
  DeoptSafepointInstr* safepoint = static_cast<DeoptSafepointInstr*>(payload);
  uintptr_t addr = uintptr_t(address);

  // Virtual pc may advance or retreat with native pc advance, so adjustment is signed.
  int32_t vpc_adjustment = int32_t(vpc_) - int32_t(safepoint->vpc);
  // Native pc values are assigned in monotonically decreasing order (due to assembly
  // in reverse), so the adjustment is always non-negative, interpreted as a decrement.
  int32_t npc_decrement = int32_t(npc_) - int32_t(addr);
  AvmAssert(npc_decrement >= 0);

  ensureSpace(1 + byteCountSLEB128(vpc_adjustment) + byteCountULEB128(npc_decrement));
  writeUInt8(MD_END);
  writeSLEB128(vpc_adjustment);
  writeULEB128(npc_decrement);
  vpc_ = safepoint->vpc;
  npc_ = addr;
}

void HMDeoptDataWriter::emitSetSlotIdx(DeoptSafepointInstr* safepoint,
                                       Assembler* assm, int slot, int i)
{
  Use& use = safepoint->value_in(i);
  int frame_idx = assm->forceStackIndex(def_ins(use));
  SlotStorageType sst = type2sst(type(use));
  // The slot type will likely change much less often than the frame_index,
  // but it is unclear at this point whether a separate MD_SLOT_TYPE instruction
  // is warranted.  For now, each MD_SLOT_IDX contains the type.
  if (frame_slots_[slot] != frame_idx || frame_types_[slot] != sst) {
    //fprintf(stderr, "slot %d @ %d : %d\n", slot, frame_idx, sst);
    ensureSpace(1 + 1 + byteCountULEB128(slot) + byteCountULEB128(frame_idx));
    writeUInt8(MD_SLOT_IDX);
    writeUInt8(uint8_t(sst));
    writeULEB128(slot);
    writeULEB128(frame_idx);
    frame_types_[slot] = uint8_t(sst);
    frame_slots_[slot] = frame_idx;
  }
}

void HMDeoptDataWriter::safepointEnd(Assembler* assm, void* payload, uint8_t* address)
{
  (void)address;
  DeoptSafepointInstr* safepoint = static_cast<DeoptSafepointInstr*>(payload);

  int n_operands = safepoint->stackp;
  int n_scopes   = safepoint->scopep;

  // TODO: Recognize nested safepoints and produce MD_INLINE.
  // This is probably better done here than in hm-liremitter.cpp.

  switch (safepoint->kind) {
  case kCallSafepoint:
    AvmAssert(safepoint->vlen < 16);
    ensureSpace(1 + 1 + byteCountULEB128(safepoint->nargs));
    writeUInt8(MD_CALL);
    writeUInt8(uint8_t((safepoint->rtype << 4) | safepoint->vlen));
    writeULEB128(safepoint->nargs);
    break;
  case kInlineSafepoint:
    ensureSpace(1+sizeof(uintptr_t));
    writeUInt8(MD_INLINE);
    writeUIntPtr(uintptr_t(safepoint->minfo));
    break;
  case kThrowSafepoint:
    ensureSpace(1);
    writeUInt8(MD_THROW);
    break;
  case kBailoutSafepoint:
    ensureSpace(1);
    writeUInt8(MD_BAILOUT);
    break;
  default:
    AvmAssert(false);
  }

  if (safepoint->stackp != stackp_) {
    stackp_ = safepoint->stackp;
    ensureSpace(1 + byteCountULEB128(stackp_));
    writeUInt8(MD_SET_STACK);
    writeULEB128(stackp_);
  }
  if (safepoint->scopep != scopep_) {
    scopep_ = safepoint->scopep;
    ensureSpace(1 + byteCountULEB128(scopep_));
    writeUInt8(MD_SET_SCOPE);
    writeULEB128(scopep_);
  }

  int j = 0;
  for (int i = 0; i < n_locals_; i++) {
    emitSetSlotIdx(safepoint, assm, i, j++);
  }
  for (int i = scope_base_; i <= n_scopes; i++) {
    emitSetSlotIdx(safepoint, assm, i, j++);
  }
  for (int i = stack_base_; i <= n_operands; i++) {
    emitSetSlotIdx(safepoint, assm, i, j++);
  }

}

void HMDeoptDataWriter::setNativePc(uint8_t* address)
{
  //fprintf(stderr, "NEW CHUNK address=0x%p\n", address);
  npc_ = uintptr_t(address);
  ensureSpace(1+sizeof(uintptr_t));
  writeUInt8(MD_SET_NPC);
  writeUIntPtr(npc_);
}

void HMDeoptDataWriter::endAssembly(Assembler* assm, uint8_t* address)
{
  (void)assm; (void)address;
  // Write sentinel value.  Not strictly necessary, but we are presently
  // depending on it in the implementation of some diagnostics.
  ensureSpace(1 + sizeof(uintptr_t));
  writeUInt8(MD_SET_NPC);
  writeUIntPtr(0);
}


/// Deoptimizer for compiled methods generated by Halfmoon.

typedef DeoptimizerInstance<DeoptData, HMDeoptContext> HMDeoptimizer;

Deoptimizer* DeoptData::createDeoptimizer(MethodInfo* info)
{
  return mmfx_new(HMDeoptimizer(info, this));
}

// Return a generic pointer to local slot at the specified index.
static void* addrOfJitSlot(uint8_t* nativefp, int slot_ix, int shift) {
  return (void*)(nativefp + (slot_ix << shift));
}

/// Frame populators for compiled methods generated by Halfmoon.

class HMFramePopulator : /*implements*/ public FramePopulator {
public:
  HMFramePopulator(AvmCore* core, const HMDeoptContext& ctx, uint8_t* nativefp)
    : core_(core),
      ctx_(ctx),
      nativefp_(nativefp)
  {}
  virtual void populate(Atom* framep, int *scopeDepth, int *stackDepth);
  virtual ~HMFramePopulator() {}
protected:
  AvmCore* core_;
  const HMDeoptContext& ctx_;
  uint8_t* nativefp_;
  SlotStorageType slotType(int i);
  void populateLocals(Atom* framep);
  void populateScopes(Atom* framep, int *scopeDepth);
  void populateStack(Atom* framep, int *stackDepth);
private:
  DISALLOW_COPY_AND_ASSIGN(HMFramePopulator);
};

SlotStorageType HMFramePopulator::slotType(int i)
{
  uint32_t tag = ctx_.frame_types_[i];
  return SlotStorageType(tag);
}

void HMFramePopulator::populateLocals(Atom* framep)
{
  int local_count = ctx_.signature_->local_count();
  for (int i = 0; i < local_count; i++) {
    void *slot_addr = addrOfJitSlot(nativefp_, ctx_.frame_slots_[i],
                                    VARSHIFT(this));
    framep[i] = nativeLocalToAtom(core_, slot_addr, slotType(i));
  }
}

void HMFramePopulator::populateScopes(Atom* framep, int *scope_depth)
{
  int scope_base  = ctx_.signature_->scope_base();

  for (int i = 0; i < ctx_.scopep_; i++) {
    int j = scope_base + i;
    void *slot_addr = addrOfJitSlot(nativefp_, ctx_.frame_slots_[j],
                                    VARSHIFT(this));
    framep[j] = nativeLocalToAtom(core_, slot_addr, slotType(j));
  }
  *scope_depth = ctx_.scopep_;
}
   
void HMFramePopulator::populateStack(Atom* framep, int *stack_depth)
{
  int stack_base  = ctx_.signature_->stack_base();

  for (int i = 0; i < ctx_.stackp_; i++) {
    int j = stack_base + i;
    void *slot_addr = addrOfJitSlot(nativefp_, ctx_.frame_slots_[j],
                                    VARSHIFT(this));
    framep[j] = nativeLocalToAtom(core_, slot_addr, slotType(j));
  }
  *stack_depth = ctx_.stackp_;
}

void HMFramePopulator::populate(Atom* framep, int *scope_depth, int *stack_depth)
{
  populateLocals(framep);
  populateScopes(framep, scope_depth);
  populateStack(framep, stack_depth);
}

class HMHandlerFramePopulator : public HMFramePopulator {
public:
  HMHandlerFramePopulator(AvmCore* core, const HMDeoptContext& ctx,
                          uint8_t* nativefp, Atom exn_val)
    : HMFramePopulator(core, ctx, nativefp),
      exn_val_(exn_val)
  {}
  virtual void populate(Atom* framep, int *scope_depth, int *stack_depth);
  virtual ~HMHandlerFramePopulator() {}
private:
  DISALLOW_COPY_AND_ASSIGN(HMHandlerFramePopulator);
  Atom exn_val_;
};

void HMHandlerFramePopulator::populate(Atom* framep, int *scope_depth,
                                       int *stack_depth)
{
  populateLocals(framep);
  *scope_depth = 0;
  int stack_base  = ctx_.signature_->stack_base();
  framep[stack_base] = exn_val_;
  *stack_depth = 1;
}
    
class HMReturnFramePopulator : public HMFramePopulator {
public:
  HMReturnFramePopulator(AvmCore* core, const HMDeoptContext& ctx,
                         uint8_t* nativefp, Atom return_val)
    : HMFramePopulator(core, ctx, nativefp),
      return_val_(return_val)
  {}
  virtual void populate(Atom* framep, int *scope_depth, int *stack_depth);
  virtual ~HMReturnFramePopulator() {}
private:
  DISALLOW_COPY_AND_ASSIGN(HMReturnFramePopulator);
  Atom return_val_;
};

void HMReturnFramePopulator::populate(Atom* framep, int *scope_depth,
                                      int *stack_depth)
{
  int stack_idx;
  populateLocals(framep);
  populateScopes(framep, scope_depth);
  populateStack(framep, &stack_idx);
  stack_idx -= ctx_.nargs_;
  int stack_base  = ctx_.signature_->stack_base();
  framep[stack_base + stack_idx] = return_val_;
  *stack_depth = stack_idx + 1;
}


/// Interpret deoptimization metadata.

HMDeoptContext::HMDeoptContext(DeoptData* data)
  : deopt_data_(data),
    signature_(data->signature_),
    frame_size_(signature_->frame_size()),
    stack_base_(signature_->stack_base()),
    scope_base_(signature_->scope_base()),
    n_locals_(signature_->local_count()),
    npc_(0),
    vpc_(0),
    scopep_(0),
    stackp_(0),
    mdins_(0)
{
  frame_slots_ = mmfx_new_array(int, frame_size_);
  frame_types_ = mmfx_new_array(uint8_t, frame_size_);
}

HMDeoptContext::~HMDeoptContext()
{
  mmfx_delete_array(frame_slots_);
  mmfx_delete_array(frame_types_);
}

uint8_t HMDeoptContext::readUInt8()
{
  return uint8_t(*(mdins_++));
}

uint32_t HMDeoptContext::readUInt32()
{
  uint32_t value = *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  return value;
}

#ifdef VMCFG_64BIT
uint64_t HMDeoptContext::readUInt64()
{
  uint64_t value = *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  value = (value << 8) | *(mdins_++);
  return value;
}
#endif

uintptr_t HMDeoptContext::readUIntPtr()
{
#ifdef VMCFG_64BIT
  return readUInt64();
#else
  return readUInt32();
#endif
}

uint32_t HMDeoptContext::readULEB128()
{
  int byte;
  uint32_t value = 0;
  do {
    byte = *(mdins_++);
    value = (value << 7) | (byte & 0x7f);
  } while (byte & 0x80);
  return value;
}

int32_t HMDeoptContext::readSLEB128()
{
  int byte;
  int shift = 0;
  uint32_t value = 0;
  do {
    byte = *(mdins_++);
    value |= (byte & 0x7f) << shift;
    shift += 7;
  } while (byte & 0x80);
  // Sign-extend
  if (byte & 0x40) {
    value |= 0xffffffff << shift;
  }
  return value;
}
  
void HMDeoptContext::setSafepointFromNativePc(uint8_t* pc)
{
  kind_ = kInvalidSafepoint;
  npc_ = 1;  // Must differ from sentinel value of 0.
  vpc_ = 0;
  scopep_ = 0;
  stackp_ = 0;
  VMPI_memset(frame_slots_, 0, frame_size_ * sizeof(int));
  VMPI_memset(frame_types_, SST_MAX_VALUE, frame_size_ * sizeof(uint8_t));
  nargs_ = 0;
  vlen_ = 0;
  rtype_ = SST_MAX_VALUE; // FIXME: bogus placeholder
  minfo_ = NULL;

  mdins_ = deopt_data_->instructions();
  //fprintf(stderr, "mdins_ = %p\n", mdins_);

  while (npc_ > uintptr_t(pc)) {
    // TODO: Need check for end of stream, if only for assertion.
    int op = *(mdins_++);
    //fprintf(stderr, "OP %d @ %p\n", op, mdins_);
    switch (op) {
    case MD_CALL: {
      kind_ = kCallSafepoint;
      uint8_t val = readUInt8();
      vlen_ = val & 0xff;
      rtype_ = SlotStorageType(val >> 4);
      nargs_ = readULEB128();
      break;
    }
    case MD_INLINE: {
      kind_ = kInlineSafepoint;
      minfo_ = (MethodInfo*)readUIntPtr();
      AvmAssert(false); // NYI
      break;
    }
    case MD_THROW: {
      kind_ = kThrowSafepoint;
      AvmAssert(false); // NYI
      break;
    }
    case MD_BAILOUT: {
      kind_ = kBailoutSafepoint;
      break;
    }
    case MD_SLOT_IDX: {
      uint8_t sst = readUInt8();
      uint32_t slot = readULEB128();
      uint32_t frame_idx = readULEB128();
      frame_slots_[slot] = frame_idx;
      frame_types_[slot] = sst;
      //fprintf(stderr, "set_slot %d @ %d\n", slot, frame_idx);
      break;
    }
    case MD_SET_STACK: {
      stackp_ = readULEB128();
      break;
    }
    case MD_SET_SCOPE: {
      scopep_ = readULEB128();
      break;
    }
    case MD_SET_NPC: {
      npc_ = readUIntPtr();
      break;
    }
    case MD_END: {
      vpc_ += readSLEB128();
      npc_ -= readULEB128();
      break;
    }
    case MD_LINK: {
      mdins_ = (NIns*)readUIntPtr();
      break;
    }
    default:
      AvmAssert(false);
    }
  }
}

Atom HMDeoptContext::interpretFromSafepoint(uint8_t* fp, MethodEnv* env)
{
  // Continue at safepoint instruction.
  AvmAssert(signature_ == env->method->getMethodSignature());
  HMFramePopulator populator(env->core(), *this, fp);
  return interpBoxedAtLocation(env, vpc_, populator);
}

Atom HMDeoptContext::interpretFromCallSafepoint(uint8_t* fp,
                                                MethodEnv* env,
                                                Atom return_val)
{
  // Interpreter frame will reconstruct state at call, with boxed result pushed
  // on operand stack.
  AvmAssert(signature_ == env->method->getMethodSignature());
  AvmAssert(isSafepointAtAbcCall());
  HMReturnFramePopulator populator(env->core(), *this, fp, return_val);
  return interpBoxedAtLocation(env, (vpc_ + vlen_), populator);
}

Atom HMDeoptContext::interpretFromThrowSafepoint(uint8_t* fp,
                                                 MethodEnv* env,
                                                 int32_t handler_vpc,
                                                 Atom exn_val) {
  // Continue at handler target, with stacks reset and exception pushed.
  AvmAssert(signature_ == env->method->getMethodSignature());
  AvmAssert(isSafepointAtThrow());
  HMHandlerFramePopulator populator(env->core(), *this, fp, exn_val);
  return interpBoxedAtLocation(env, handler_vpc, populator);
}

} // end namespace halfmoon

#endif // VMCFG_HALFMOON
