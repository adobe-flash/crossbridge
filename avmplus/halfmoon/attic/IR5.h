/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef IR5_H_
#define IR5_H_

#include "opt-impl.h"

namespace halfmoon {
using namespace avmplus;

/// Features
///
/// goto-with-args style for merges and splits.
///
/// Every instruction is part of a circular list which forms a basic block.
/// Basic blocks start with Start/Edge/Label and end with Goto/If/Stop.
///
/// Variable-arity arrays are used for split and merged variables (new!).
///
/// A Goto always uniquely identifies an in-edge at a merge point.  An Edge
/// always uniquely identifies an out-edge at a branch point.
///
/// Maybe:
///  * Start = Label w/ empty preds list, or Edge with empty
///  * Stop = Goto with null target
///
struct IR5 {
  struct Instr;
  struct Stmt;
  struct BlockStart;
  struct   Start;
  struct   Label;
  struct   Edge;
  struct BlockEnd;
  struct   If;
  struct   Goto;
  struct   Stop;

  // There can be only one.
  Start* start;
  Stop* stop;

  struct Instr {
    int id;
    InstrInfo* info;
  };

  // A BlockStart delimits the beginning of a linear sequence of
  // executable instructions.
  //
  struct BlockStart : Instr {
    int block_id;             // dense numbering, because instr->id is sparse
    Def* params;              // block data inputs.
    Instr* next;              // beginning of block's executable instruction list
    BlockEnd* end;            // end of this block.
  };

  // A Start begins a block that is only accessed by an indirect call.
  //
  // A Start's params are the bona fide parameters of the callable 
  // entity. No direct references to a Start will appear in an IR graph,
  // but indirect calls must supply a sequence of arguments which is
  // type- and shape-compatible with the start's param sequence.
  // 
  struct Start : BlockStart {
  };

  // An Edge begins a block that is owned by a Branch, a BlockEnd
  // that represents an act of data-driven selection among several 
  // subsequent computations (If, Switch are Branches). An Edge 
  // has exactly one predecessor, its owner. 
  //
  // An edge's params are type-refined values transferred by the 
  // owner branch to this edge upon selection. All edges of a given 
  // branch have the same sequence of params (mod type refinements), 
  // and these are congruent with the branch's args (thus the number
  // of params carried by each edge is given by the branch's argument
  // count).
  // 
  struct Edge : BlockStart {
    If* owner;                // branch that owns this edge.
  };

  // A Label begins an labeled (i.e., addressable) block. By virtue
  // of addressability, a label may be the target of multiple 
  // predecessor Gotos.
  //
  // A label's params are values transferred to the label by a 
  // predecessor Goto. The number of params (and hence the number 
  // of args supplied by each predecessor) is given by argc.
  // 
  struct Label : BlockStart {
    Goto* preds;                  // list of incoming gotos
    int argc;                     // number of merged variables.
  };

  // A BlockEnd delimits the end of a linear sequence of
  // executable instructions.
  //
  struct BlockEnd : Instr {
    Instr* prev;                  // end of block's executable instruction list
    BlockStart* start;            // start of this block.
    Use* args;                    // output values to next block or return
  };

  // A Goto ends a block with a transition to a target Label,
  // an addressable block with a variable number of predecessors.
  //
  // A goto's args specify values for the target Label's params.
  // The two sequences are congruent, so a goto's arg count is
  // its target's param count.
  //
  struct Goto: BlockEnd {
    Label* target;                // target Label for this Goto
    Goto *next_goto, *prev_goto;  // other Gotos to same target
  };

  // An If ends a block with a transition to one of a fixed
  // set of successor Edges. As described here, If represents 
  // both if and switch statements, where the former branch over
  // two-valued selectors, and the latter branch over n-valued
  // selectors and carry a default. The choice of edge is determined
  // by the selector.
  // 
  struct If : BlockEnd {          // includes switch
    Use cond;                     // conditional selector
    int argc;                     // number of split variables.
    Edge succs[2];                // arity based on opcode. if=2, switch=N
  };

  // A Stop instruction delimits an executable block. Its
  // arguments match the block's signature of returned values.
  // 
  struct Stop : BlockEnd {
  };

  // A Stmt is an executable instruction with no transition
  // semantics. Lists of Stmts denote sequential execution.
  //
  struct Stmt: Instr {
    Instr *prev, *next;
  };

  // algorithms & iterators
  //
  // fold If with constant-condition  (fold IF->Edge)
  // 1. pick the live edge E.
  // 2. connect uses of E.vars to IF.args
  // 3. combine blocks, remove If and Edge
  //
  // block straighten (fold Goto->Label)
  // 1. pick the single incoming Goto
  // 2. connect uses of Label.vars to Goto.args
  // 3. combine blocks, remove Goto and Label
  //
  // EachPhiUse(Goto *G) - visit each var Use along a merge edge
  // for (i=0, n = G->target->argc; i < n; i++) { G->args[i] }
  //
  // PhiReduce(Label *label, int V) - visit incoming Uses for merged var V
  // for (Goto* g = label->preds; g; g = g->next_goto) { g->vars[V] }
  //
  // Add or remove split/phi:
  // 1. find position V in args/vars array of if/label
  // 2. for all succs/preds, shift edge/goto's vars/args up/down
  // 3. inc/dec if/label's argc.
  //
  // AbcBuilder::addEdge()
  // jump:
  //   add goto[framevars], save current state
  // if/switch:
  //   1. save frame in if.args
  //   2. init Edge.vars from current state, plus type refinement.
  //   3. add Goto[framevars], point to edge.vars for split vars
  // addFirst/JoinEdge() doesn't need to do anything special
  //
  // AbcBuilder::startBlock()
  // - if !loop and 1 pred, fold
  // - no phis to create now or remove later.
  //
  // propagateTypes()
  // - phi handling is all-at-once on Label
  // - could re-queue just instrs that use defs that changed
  // - if has no defs. enque only reachable edges
  // - goto has no defs; enque label
  //
  // PostorderDefIter
  // - visit normal instructions same as before
  // - Stop & Goto have use arrays, can Stack ArrayRange like before
  // - Edge: visit If then self
  // - Label: stack up ranges for each predecessor, then self
  //
  // #Edges successors   predecessors
  // ------ ----------   ------------
  // 0      Stop - n/a   Start - n/a
  // 1      Goto.target  Edge.owner
  // 2+     If.succs     Label.preds (LL)
  //
  // Instr  #Defs        #Uses
  // -----  ---------    ---------
  // Start  param-count  0
  // Edge   owner->argc  0
  // Label  argc         0
  // Stop   0            return-count
  // Goto   0            target->argc
  // If     0            1 + argc (cond + vars)
  //
  // Possible issues:
  //
  // LirEmitter and InterpFrame need def-arity to be fixed for the duration
  // of processing an IR.  Assigning def ids with variable-arrays of defs
  // might be a pain, with the def_id = instr_id + K scheme.
  //
};

}

#endif // IR5_H_
