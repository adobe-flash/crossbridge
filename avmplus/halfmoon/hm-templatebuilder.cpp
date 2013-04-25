/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON
#include "hm-templatebuilder.h"

namespace halfmoon {
using nanojit::BitSet;
using avmplus::Toplevel;
using avmplus::getBinding;

///
/// TemplateBuilder
///
TemplateBuilder::TemplateBuilder(InstrFactory* factory)
: factory_(*factory)
, ir_(factory->createGraph())
, builder_(ir_, factory)
, start_instr_(0) {
}

/// start a template with a template instruction. 
/// template shape comes from instr_attrs on the given opcode.
/// TODO resolve ambiguity between using a variadic
/// representation and bona fide variadic template params
///
void TemplateBuilder::start(InstrKind k, int num_params,
                            const Type* param_types[]) {
  assert(factory_.hasTemplate(k) && "instruction is not a template"); 
  (void)k;
  start_instr_ = factory_.newStartInstr(HR_template, num_params, param_types);
  ir_->begin = (StartInstr*)builder_.addInstr(start_instr_);
}

} // namespace avmplus
#endif // VMCFG_HALFMOON
