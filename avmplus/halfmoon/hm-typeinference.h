/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace halfmoon {

/**
 * Sparse Conditional Constant propagation.  Reset all types of all defs,
 * then recompute types using the classic worklist algorithm.
 * TODO: literature reference.
 */
void propagateTypes(InstrGraph*);

/**
 * Calculate the types of an IR graph with respect to the
 * specified argument types, rather than the graph start
 * instruction's stated param types.
 * 
 * Calculated types are left in the graph, with the exception
 * that the graph's param types are restored after the calculation.
 * Calculated output types may be retrieved from the graph's return
 * instruction. Note: it's the caller's responsibility to copy
 * calculated results to a permanent location: they will be
 * overwritten the next time this function is called on the graph.
*/
void propagateTypes(InstrGraph* ir, const Type** arg_types);

/**
 * Propagate types through a fat instruction subgraph,
 * starting from the input types of an instance.
 * 
 * Note: propagated type are left in place in the subgraph;
 * caller must copy them out to a permanent location.
 */
void propagateTypes(Instr* fat_instr);

}
