/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef TEMPLATE_BUILDER_H_
#define TEMPLATE_BUILDER_H_

namespace halfmoon {
using nanojit::BitSet;
using avmplus::ExceptionHandler;

/// A TemplateBuilder is used for creating InstrGraphs for
/// small instruction templates.
///
class TemplateBuilder {
public:
  TemplateBuilder(InstrFactory* factory);

  /// start the given template
  /// TODO handle multiple control inputs, data or control varargs
  void start(InstrKind k, int num_params, const Type* param_types[]);

  /// return a reference to the given template param
  Def* paramRef(int i) {
    assert(i >= 0 && i < numDefs(start_instr_));
    return &start_instr_->params[i];
  }

  /// add instr to template, return pointer to instr.
  /// We also check for return being added here,
  /// and set IR's end instr when we see it.
  ///
  Instr* addInstr(Instr* instr) {
    instr = builder_.addInstr(instr);
    if (kind(instr) == HR_return) {
      assert(!ir_->end);
      ir_->end = cast<StopInstr>(instr);
    }
    return instr;
  }

  /// add type const-generating instruction to template
  Def* traitsConst(Traits* traits) {
    return builder_.addConst(ir_->lattice.makeTraitsConst(traits));
  }

  /// add int const-generating instruction to template
  Def* intConst(int c) {
    return builder_.addConst(ir_->lattice.makeIntConst(c));
  }

  /// add double const-generating instruction to template
  Def* doubleConst(double c) {
    return builder_.addConst(ir_->lattice.makeDoubleConst(c));
  }

  /// return a reference to the finished template IR
  InstrGraph* ir() {
    assert(ir_->end && "incomplete template");
    return ir_;
  }

private:
  InstrFactory& factory_;
  InstrGraph* ir_;
  InstrGraphBuilder builder_;
  StartInstr* start_instr_;
};

} // namespace halfmoon

#endif // TEMPLATE_BUILDER_H_
