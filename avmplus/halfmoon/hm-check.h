/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace halfmoon {

/// Run checkTypes(Instr) on every instruction reachable by PostorderDefIter.
///
bool checkTypes(InstrGraph*, bool check_model);

/// Check that the inputs to Instr have legal types.
///
bool checkTypes(Instr*, bool check_model);

/// Check that InstrGraph is well formed.
///
/// There must be exactly one Begin and one End instr.  Arguments cannot
/// be CTRL, and control inputs must be CTRL or UN.
///
/// Compute the set of uses for each Def by inspecting defs, then make sure
/// it maches the use list in each Def.
///
bool checkUses(InstrGraph*);

/// Ensure the graph has a begin instruction and either a return or throw
/// instruction, or both.
bool checkStartEnd(InstrGraph*);

/// Ensure that the ir has exactly one end instruction of the required opcode.
///
bool checkOneEnd(InstrGraph* ir, Instr* end, InstrKind k);

/// Check that each result type is defined.  This catches a mistake
/// where SomeInstr.computeTypes() fails to assign a type to every def.
///
bool checkResultTypes(Instr* n, bool check_model);

bool checkPruned(InstrGraph* ir);
bool checkSSA(InstrGraph* ir);

/// Check that the graph is fully linked: mark all linked instructions, then
/// assert that no unmarked instructions can be reached from a label->goto
/// edge, a def->use edge, or a use->def edge.
bool checkPruned(InstrGraph* ir);

bool checkSSA(InstrGraph* ir);

} // namespace avmplus
