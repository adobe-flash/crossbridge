/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_PRETTYPRINT_H_
#define HM_PRETTYPRINT_H_

namespace halfmoon {

/// Print the details of one instruction.
///
PrintWriter& printInstr(PrintWriter&, Instr*);
void printInstr(Instr*);

/// Print the brief essentials of one instruction.
///
PrintWriter& printCompactInstr(PrintWriter&, Instr*, bool print_defs = true);
void printCompactInstr(Instr*, bool print_defs = true);

/**
 * Print list of instructions in detail.
 * @param list head of list of instructions.
 * @param ir instruction graph
 * @param title legend printed at head of list.
 * A correctly formed list is circular and the end of the list (and printout) is when the next instruction
 * points back at the list head. To prevent debug code from looping when confronted with incorrect lists there
 * is a limit to the number of instructions that will be printed.
 */
void printInstrList(InstrRange list, InstrGraph *ir, const char* title);

/// Print list of Instr ids, with title: "title=[n1,n2,...nN]".
/// Note: prints "" for empty list.
/// There are no restrictions on what the first and last instructions are,
/// (blocks or jumps).  This prints the list on one line with just the id and
/// opcode; easy on the eyes when rearranging blocks.
/// A correctly formed list is circular and the end of the list (and printout) is when the next instruction
/// points back at the list head. To prevent debug code from looping when confronted with incorrect lists there
/// is a limit to the number of instructions that will be printed.
///
void printTerseInstrList(InstrRange, InstrGraph *ir, const char* title);

inline void printTerseInstrList(Instr* list, InstrGraph* ir,
                                const char* title) {
  printTerseInstrList(InstrRange(list), ir, title);
}

/// Dump IR graph to GML and maybe console.
/// Called after each optimization phase.
///
void printGraph(Context*, InstrGraph* g, const char *phase);

/// Discover the cfg, then schedule un-ordered instructions
/// and blocks as follows:
/// 1. discover blocks by traversing control edges in preorder.
/// 2. compute the reverse-postorder list of blocks
/// 3. compute immediate dominators
/// 4. schedule floating instructions into the lowest block
///    that dominates all uses of the expression
/// 5. if there are loops, scheduleBlocks by making loop body blocks
///    adjacent, before blocks that come after the loop.
///
void listCfg(PrintWriter& console, InstrGraph*);

/// Just list the instructions without doing a CFG analysis.
void listInstr(PrintWriter& console, InstrGraph*);


/// Print the ABC control flow graph, plus the IR graphs, called immediately
/// after building the IR graph.
///
void printAbcCfg(MethodInfo*, Seq<AbcBlock*>*, InstrGraph*, const char* phase);

/// Print one ABC instruction; just a wrapper for AvmCore::formatInstr.
///
void printAbcInstr(MethodInfo* m, PrintWriter& console, const uint8_t* pc);

/// Return kJoinPrefix if block has two or more predecessors,
/// otherwise return kBlockPrefix.
///
const char* label(BlockStartInstr* block);

// Prefixes to use when printing instructions.
extern const char* kBlockPrefix;    // Prefix for a simple block.
extern const char* kLoopPrefix;     // Prefix for a loop header.
extern const char* kJoinPrefix;     // Prefix for a non-loop merge block.
extern const char* kInstrPrefix;    // Prefix for an instruction.
extern const char* kDefPrefix;      // Prefix for a def.

/// Print the def as kDefPrefix + owner->id + field name.
///
void printDef(PrintWriter& out, const Def* def);
void printDef(const Def* def);

/// Print just the name of the def.  fixme: field is a historical term, change it.
///
void printDef(PrintWriter& out, const Def& ref);
void printDef(const Def& ref);

/// Return the pretty-print name of this instruction's opcode.
///
const char* name(Instr*);
const char* name(Instr&);

/// Return the name of this Def's type.
///
const char* typeName(const Def&);
const char* typeName(const Use&);
inline const char* typeName(const Def* d) {
  return typeName(*d);
}
const char* typeName(const Type* t);

/** Print a detailed description of i. */
void print(PrintWriter& out, Instr*);

/** Write LIR control flow graph to GML file. */
void printLirCfg(MethodInfo* method, Fragment* frag);

}

#endif // HM_PRETTYPRINT_H_
