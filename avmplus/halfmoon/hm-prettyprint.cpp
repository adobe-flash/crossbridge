/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using avmplus::StringBuffer;

const char* kBlockPrefix  = "B";
const char* kLoopPrefix   = "L";
const char* kJoinPrefix   = "J";
const char* kInstrPrefix  = "i";
const char* kDefPrefix    = "d";

FILE* openFile(MethodInfo* m, const char* phase, const char *ext) {
  Allocator alloc;

#ifdef AVMPLUS_VERBOSE
  StringBuffer buf(m->pool()->core);
  buf << m << "-" << phase << ext;
  char* filename = new (alloc) char[1 + VMPI_strlen(buf.c_str())];
  VMPI_strcpy(filename, buf.c_str());
#else
  size_t phase_length = VMPI_strlen(phase);
  size_t ext_length = VMPI_strlen(ext);
  size_t total_length = phase_length + ext_length;
  char* filename = new (alloc) char[1 + total_length];
  VMPI_strncpy(filename, phase, phase_length); 
  VMPI_strncat(filename, ext, ext_length);
  (void) m; // What do we do with method?
#endif

  const char *illegal = "/<>$:";
  const char *legal = ".()__";
  for (char *s = filename; *s; ++s) {
    const char *p = VMPI_strchr(illegal, *s);
    if (p)
      *s = legal[p - illegal];
  }
  FILE* f = fopen(filename, "w");

#if !defined(_WIN32) && !defined(_WIN64)
  // TODO should do something for win builds (ShellCore also)
  setlinebuf(f);
#endif

  return f;
}

//
// Graph Drawing Conventions:
//
// solid = Use->Def dependence edge.
// dashed = any other kind of edge.
//
// Red = control flow
// Blue = "state" value (linear types).
// Black = ordinary scalar value.
// Gray = other administrative edge
//
// Round-rect = ordinary instruction or basic block
// Trapezoids = block starts/ends.  Long side is for uses/defs, skinny side
// is for control-flow.
//

// edge colors
static const char* state_color  = "#3366FF"; // Lt Blue.
static const char* edge_color  = "#FF0000"; // Red.
static const char* data_color  = "#000000"; // Black.
static const char* un_color    = "#AAAAAA"; // Gray.

/// print GML group node, return gid
///
int printGroupNode(FILE* f, int& id_offset, const char* label) {
  int gid = id_offset;
  id_offset++;

  fprintf(f, "  node [ id %d\n", gid);
  fprintf(f, "    label \"%s\"\n", label);
  fprintf(f, "    graphics [\n");
  fprintf(f, "      fill  \"#CAECFF84\"\n");
  fprintf(f, "      hasOutline 0\n");
  fprintf(f, "    ]\n");
  fprintf(f, "    LabelGraphics [\n");
  fprintf(f, "      fill  \"#99CCFF\"\n");
  fprintf(f, "      autoSizePolicy \"node_width\"\n");
  fprintf(f, "      anchor \"t\"\n");
  fprintf(f, "      borderDistance 0.0\n");
  fprintf(f, "    ]\n");
  fprintf(f, "    isGroup 1\n");
  fprintf(f, "  ]\n");

  return gid;
}

/// Print forward or reverse dominators tree of ir.  We compute the trees
/// on the fly, just to print it.
void printDomTree(FILE* f, int& id_offset, InstrGraph* ir, bool forward) {
  Allocator scratch;

  // print forward dominator tree with edges from root towards leaves
  DominatorTree* doms = forward ? forwardDoms(scratch, ir) : reverseDoms(scratch, ir);
  const bool kPrintDepth = false; // set to true to display dom depth.
  int gid = printGroupNode(f, id_offset, forward ? "dom" : "rdom");
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    fprintf(f, "  node [ id %d\n", block->blockid + id_offset);
    if (kPrintDepth)
      fprintf(f, "    label \"%s%d\n%d\"\n", kBlockPrefix, block->blockid,
              doms->depth(block));
    else
      fprintf(f, "    label \"%s%d\"\n", kBlockPrefix, block->blockid);
    fprintf(f, "    graphics [\n");
    fprintf(f, "      type \"roundrectangle\"\n");
    fprintf(f, "    ]\n");
    fprintf(f, "    gid %d\n", gid);
    fprintf(f, "  ]\n");
  }
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    if (!doms->hasIDom(block))
      continue;
    int source = forward ? doms->idom(block)->blockid : block->blockid;
    int target = forward ? block->blockid : doms->idom(block)->blockid;
    fprintf(f, "  edge [ source %d target %d ]\n",
            source + id_offset,
            target + id_offset);
  }
  id_offset += ir->block_count() + 1;
}

/// Print the loop tree, computed on the fly.
///
void printLoopTree(FILE* f, int& id_offset, InstrGraph* ir) {
  Allocator scratch;
  LoopTree loops(scratch, ir, forwardDoms(scratch, ir));

  // print loop tree with edges from root towards leaves
  const bool kPrintDepth = true; // set to true to display loop depth.
  int gid = printGroupNode(f, id_offset, "loops");
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    fprintf(f, "  node [ id %d\n", block->blockid + id_offset);
    if (kPrintDepth) {
      fprintf(f, "    LabelGraphics [\n");
      fprintf(f, "      text      \"%s%-3d%d\"\n", kBlockPrefix, block->blockid,
              loops.depth(block));
      fprintf(f, "      alignment \"left\"\n");
      fprintf(f, "      fontName  \"Consolas\"\n");
      fprintf(f, "      anchor    \"tl\"\n"); // Top Left within the node.
      fprintf(f, "    ]\n");
    } else {
      fprintf(f, "    label \"%s%d\"\n", kBlockPrefix, block->blockid);
    }
    fprintf(f, "    graphics [\n");
    fprintf(f, "      type \"roundrectangle\"\n");
    fprintf(f, "    ]\n");
    fprintf(f, "    gid %d\n", gid);
    fprintf(f, "  ]\n");
  }
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    BlockStartInstr* parent = loops.parent(block);
    if (!parent)
      continue;
    fprintf(f, "  edge [ source %d target %d ]\n", parent->blockid + id_offset,
            block->blockid + id_offset);
  }
  id_offset += ir->block_count() + 1;
}

// print a control-flow-graph edge.
void printCfgEdge(FILE* f, int source_id, int target_id, int label = -1) {
  fprintf(f, "  edge [\n");
  fprintf(f, "    target %d\n", target_id);
  fprintf(f, "    source %d\n", source_id);
  fprintf(f, "    graphics [\n");
  fprintf(f, "      arrow \"last\"\n");
  fprintf(f, "      fill \"%s\"\n", edge_color);
  fprintf(f, "    ]\n");
  if (label != -1) {
    fprintf(f, "    LabelGraphics [\n");
    fprintf(f, "      text \"%d\"\n", label);
    fprintf(f, "      model \"three_center\"\n");
    fprintf(f, "      position \"scentr\"\n");
    fprintf(f, "    ]\n");
  }
  fprintf(f, "  ]\n");
}

// print InstrGraph's embedded CFG
void printGmlCfg(FILE* f, MethodInfo* m, int& id_offset, InstrGraph* ir,
                 bool print_code = true) {
  // group node
  int gid = printGroupNode(f, id_offset, print_code ? "cfg-full" : "cfg");

  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    int id = block->blockid + id_offset;
    fprintf(f, "  node [\n");
    fprintf(f, "    id %d\n", id);
    fprintf(f, "    graphics [\n");
    fprintf(f, "      type \"roundrectangle\"\n");
    fprintf(f, "    ]\n");
    StringBuffer buf(m->pool()->core);
    buf << kBlockPrefix << block->blockid << "\n";
    if (print_code)
      for (InstrRange i(b.front()); !i.empty(); i.popFront())
        printCompactInstr(buf, i.front());
    fprintf(f, "    LabelGraphics [\n");
    fprintf(f, "      text      \"%s\"\n", buf.c_str());
    fprintf(f, "      alignment \"left\"\n");
    fprintf(f, "      fontName  \"Consolas\"\n");
    fprintf(f, "      anchor    \"tl\"\n"); // Top Left within the instr.
    fprintf(f, "    ]\n");
    fprintf(f, "    gid %d\n", gid);
    fprintf(f, "  ]\n");
  }
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    int id = block->blockid + id_offset;
    BlockEndInstr* end = ir->blockEnd(block);
    switch (kind(end)) {
    case HR_goto: {
      GotoInstr* go = cast<GotoInstr>(end);
      printCfgEdge(f, id, id_offset + go->target->blockid);
      break;
    } 
    case HR_if:
    case HR_switch: {
      CondInstr* instr = (CondInstr*)end;
      for (ArrayRange<ArmInstr*> r = armRange(instr); !r.empty();)
        printCfgEdge(f, id, id_offset + r.popFront()->blockid);
      break;
    } 
    }
  }
  id_offset += ir->block_count();
}

// print edge from one Instr to another. Edge type is
// figured out by caller and display settings are passed in.
void printEdge(FILE* f, int id_offset, Instr* from, Instr* to,
               const char* color, const char* style, int width,
               int from_label = -1) {
  fprintf(f, "  edge [\n");
  fprintf(f, "    source %d\n", id_offset + from->id);
  fprintf(f, "    target %d\n", id_offset + to->id);
  fprintf(f, "    graphics [\n");
  fprintf(f, "      arrow \"last\"\n");
//  fprintf(f, "      sourceArrow \"standard\"\n");
//  fprintf(f, "      targetArrow \"none\"\n");
  fprintf(f, "      width %d\n", width);
  fprintf(f, "      style \"%s\"\n", style); // "solid", "dashed", "dotted"
  fprintf(f, "      fill \"%s\"\n", color);
  fprintf(f, "    ]\n");
  if (from_label >= 0) {
    fprintf(f, "    LabelGraphics [\n");
    fprintf(f, "      text \"%d\"\n", from_label);
    fprintf(f, "      model \"three_center\"\n");
    fprintf(f, "      position \"scentr\"\n");
    fprintf(f, "      fontStyle \"bold\"\n");
    fprintf(f, "    ]\n");
  }
  fprintf(f, "  ]\n");
}

// print edge from one Instr to (a Def in) another.
// Display settings are driven by Def being pointed to.
// void printEdge(FILE* f, int id_offset,  Def* def, Instr* instr)
void printEdge(FILE* f, int id_offset, const Use& use) {
  Instr* to = user(use);
  Instr* from = definer(use);
  const Type* t = type(use);
  int e = numDefs(from) > 1 ? pos(def(use)) : -1;
  if (isEffect(t))
    printEdge(f, id_offset, from, to, state_color, "solid",  2, e);
  else if (isBottom(t))
    printEdge(f, id_offset, from, to, un_color,   "solid",  1);
  else if (isState(t))
    printEdge(f, id_offset, from, to, state_color, "solid", 1);
  else
    printEdge(f, id_offset, from, to, data_color, "solid",  1, e);
}

void printNode(FILE* f, int id_offset, Instr* instr, MethodInfo* m, int gid) {
  StringBuffer label(m->pool()->core);
  label << kInstrPrefix << instr->id
        << (instr->id < 10 ? "  " : " ")
        << name(instr) << "\n    ";
  if (kind(instr) == HR_const)
    label << typeName(getDefs(instr)[0]);
  else
    print(label, instr);
  fprintf(f, "  node [ id %d\n", instr->id + id_offset);
  fprintf(f, "    graphics [\n");

  const char *shape;
  const char *fill;
  if (isBlockStart(instr)) {
    shape = "trapezoid";
    fill = "#FF9900";
  } else if (isBlockEnd(instr)) {
    shape = "trapezoid2";
    fill = "#FF9900";
  } else {
    shape = "roundrectangle";
    fill = "#CCCCFF";
  }

  fprintf(f, "      type \"%s\"\n", shape);
  fprintf(f, "      fill \"%s\"\n", fill);

  fprintf(f, "    ]\n");
  fprintf(f, "    LabelGraphics [\n");
  fprintf(f, "      text      \"%s\"\n", label.c_str());
  fprintf(f, "      alignment \"left\"\n");
  fprintf(f, "      fontName  \"Consolas\"\n");
  fprintf(f, "      anchor    \"tl\"\n"); // Top Left within the instr.
  fprintf(f, "    ]\n");
  fprintf(f, "    gid %d\n", gid);
  fprintf(f, "  ]\n");
}

// fwd
void printGmlInstrGraph(FILE*, int& id_offset, InstrGraph*, MethodInfo*, const char* label = "ir");

// print subgraph once
void printSubgraph(FILE* f, int& id_offset, MethodInfo* m,
                   HashMap<InstrGraph*, bool>& subs,
                   InstrGraph* ir, const char* label) {
  if (!subs.containsKey(ir)) {
    subs.put(ir, true);
    printGmlInstrGraph(f, id_offset, ir, m, label);
  }
}

// Print the instructions and edges.  All edges are drawn in the opposite
// order of the underlying pointer, with the idea that if USE points to DEF,
// then USE depends on DEF, and thus DEF "precedes" USE.  The direction of
// the arrows shows the "precedes" relation.
//
// Trust me, it looks better this way, and lays out better in yEd this way.
void printGmlInstrGraph(FILE* f, int& id_offset, InstrGraph *ir, MethodInfo* m,
                        const char* label) {
  Allocator scratch;
  HashMap<InstrGraph*, bool> subs(scratch);

  // group node
  int gid = printGroupNode(f, id_offset, label);

  int new_offset = id_offset + ir->size();

  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    printNode(f, id_offset, instr, m, gid);
    if (hasSubgraph(instr))
      printSubgraph(f, new_offset, m, subs, subgraph(instr), name(instr));
  }
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    // print def->use edges
    for (ArrayRange<Use> r = useRange(instr); !r.empty(); r.popFront())
      printEdge(f, id_offset, r.front()); 
    // print control edges
    switch (kind(instr)) {
      case HR_goto: {
        GotoInstr* go = cast<GotoInstr>(instr);
        printEdge(f, id_offset, go, go->target, edge_color, "dashed", 1);
        break;
      }
      case HR_if:
      case HR_switch: {
        for (ArrayRange<ArmInstr*> r = armRange((CondInstr*) instr); !r.empty();)
          printEdge(f, id_offset, instr, r.popFront(), edge_color, "dashed", 1);
        break;
      }
    }
    if (isBlockStart(instr))
      printEdge(f, id_offset, instr, ir->blockEnd((BlockStartInstr*)instr),
                edge_color, "dashed", 1);
  }
  // add a light gray edge from constants to the block they are in, to
  // help get a better layout.
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    Instr* block = b.front();
    for (InstrRange i(block); !i.empty(); i.popFront()) {
      Instr* instr = i.front();
      if (numUses(instr) == 0 && instr != block)
        printEdge(f, id_offset, block, instr, "#EEEEEE", "dashed", 1);
    }
  }
  id_offset = new_offset;
}

/** Print the InstrGraph in Graph Modelling Language (GML) */
void printGml(InstrGraph* ir, MethodInfo* m, const char *phase) {
  FILE *f = openFile(m, phase, ".gml");
  fprintf(f, "graph [ directed 1 hierarchic 1\n");
  int id_offset = 0;

  // Print plain instr graph.
  printGmlInstrGraph(f, id_offset, ir, m);
  printGmlCfg(f, m, id_offset, ir, true); // cfg with code
  printGmlCfg(f, m, id_offset, ir, false); // cfg without code
  printDomTree(f, id_offset, ir, true); // forward
  printDomTree(f, id_offset, ir, false); // backward
  printLoopTree(f, id_offset, ir);

  fprintf(f, "]\n");
  fclose(f);
}

void printGraph(Context* cxt, InstrGraph* ir, const char *phase) {
  if (enable_verbose) {
    cxt->out << "---- after pass " << phase << ": linked instructions ----\n";
    listInstr(cxt->out, ir);
  }
  if (enable_gml)
    printGml(ir, cxt->method, phase);
}

// print graph of blocks as parsed from ABC
void printAbcBlocks(FILE* f, MethodInfo* m, int& id_offset,
                    Seq<AbcBlock*>* abc_blocks) {
  // must use block count + 1 as group node id, because block post_ids are fixed
  for (SeqRange<AbcBlock*> b(abc_blocks); !b.empty(); b.popFront())
    id_offset++;

  // group node
  int gid = printGroupNode(f, id_offset, "abc");

  for (SeqRange<AbcBlock*> b(abc_blocks); !b.empty(); b.popFront()) {
    AbcBlock* block = b.front();
    fprintf(f, "  node [ id %d\n", block->post_id);
    StringBuffer label(m->pool()->core);
    label << (block->num_preds > 1 ? block->dfs_loop ?
        kLoopPrefix : kJoinPrefix : kBlockPrefix)
          << block->post_id << "\n";
    for (const uint8_t* pc = block->start; pc < block->end;) {
      printAbcInstr(m, label, pc);
      uint32_t imm30, imm30b;
      int imm8, imm24;
      uint8_t abcop = *pc;
      AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
      if (abcop == OP_lookupswitch)
        pc += 3 * (imm30b + 1);
    }
    fprintf(f, "    graphics [ fill \"#99CCFF\" ]\n"); // light blue
    fprintf(f, "    LabelGraphics [\n");
    fprintf(f, "      alignment \"left\"\n");
    fprintf(f, "      fontName  \"Consolas\"\n");
    fprintf(f, "      anchor    \"tl\"\n"); // Top Left within the instr.
    fprintf(f, "      text      \"%s\"\n", label.c_str());
    fprintf(f, "    ]\n");
    fprintf(f, "    gid %d\n", gid);
    fprintf(f, "  ]\n");
  }
  for (SeqRange<AbcBlock*> b(abc_blocks); !b.empty(); b.popFront()) {
    AbcBlock* block = b.front();
    for (int i = 0, n = block->num_succ_blocks; i < n; ++i)
      fprintf(f, "  edge [ source %d target %d ]\n", block->post_id,
              block->succ_blocks[i]->post_id);
    for (int i = 0, n = block->max_catch_blocks; i < n; ++i) {
      AbcBlock* succ = block->catch_blocks[i];
      if (!succ) continue;
      fprintf(f, "  edge [ source %d target %d ]\n", block->post_id,
              block->catch_blocks[i]->post_id);
    }
  }
}

void printAbcCfg(MethodInfo* m, Seq<AbcBlock*>* abc_blocks,
                 InstrGraph* ir, const char* phase)
{
  FILE* f = openFile(m, phase, ".gml");
  fprintf(f, "graph [ directed 1 hierarchic 1\n");
  int id_offset = 1;

  // print control flow graph of ABC blocks
  printAbcBlocks(f, m, id_offset, abc_blocks);
  // print IR graph
  printGmlInstrGraph(f, id_offset, ir, m);
  // print CFG from IR
  printGmlCfg(f, m, id_offset, ir, true); // cfg with code
  printGmlCfg(f, m, id_offset, ir, false); // cfg without code
 // print dominator trees from IR
  printDomTree(f, id_offset, ir, true); // forward
  printDomTree(f, id_offset, ir, false); // backward
  printLoopTree(f, id_offset, ir);

  fprintf(f, "]\n");
  fclose(f);
}

//
// Pretty Printing
//

template<class RANGE>
static const char* delim(const RANGE& r) {
  RANGE s = r;
  s.popFront();
  return s.empty() ? "" : " ";
}

void printInstrList(InstrRange list, InstrGraph *ir, const char *title) {
  if (!enable_verbose)
    return;
  PrintWriter &console = ir->lattice.console();
  console << " " << title << "\n";
  int limit = 0;
  for (; !list.empty(); list.popFront()) {
    printInstr(console, list.front());
    if (limit++ > 1000) {
      console << "printInstrList limit reached\n";
      break;
    }
  }
  console << "]\n";
}

void printTerseInstrList(InstrRange list, InstrGraph *ir, const char *title) {
  if (!enable_verbose || list.empty())
    return;
  PrintWriter &console = ir->lattice.console();
  Instr* i = list.front();
  console << " " << title << "=[" << kInstrPrefix << i->id << "-" << name(i);
  int limit = 1;
  for (list.popFront(); !list.empty(); list.popFront()) {
    i = list.front();
    console << "," << kInstrPrefix << i->id << "-" << name(i);
    if (limit++ > 100)
      console << "printTerseInstrList limit reached";
  }
  console << "]\n";
}

PrintWriter& printUsers(PrintWriter& console, Instr* instr) {
  printInstr(console, instr);

  // Print instructions that use any def of this instruction.
  for (AllUsesRange uses(instr); !uses.empty(); uses.popFront()) {
    printInstr(console, user(uses.front()));
  }
  return console;
}

void printUsers(Instr* instr) {
  printUsers(avmplus::AvmCore::getActiveCore()->console, instr);
}


PrintWriter& printInstr(PrintWriter& console, Instr* instr) {
  char namebuf[80];
  sprintf(namebuf, "    %s%-3d %-18.18s  ", kInstrPrefix, instr->id, name(instr));
  console << namebuf;
  print(console, instr);

  // Print signature of this instruction: (uses) -> (defs)
  console << "(";
  for (ArrayRange<Use> u = useRange(instr); !u.empty(); u.popFront()) {
    printDef(console, def(u.front()));
    console << ":" << typeName(u.front()) << delim(u);
  }
  console << ") -> (";
  for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront()) {
    printDef(console, d.front());
    if (numDefs(instr) > 1)
      console << ":";
    console << typeName(d.front()) << delim(d);
  }
  console << ")";

  // Print instructions that use any def of this instruction.
  AllUsesRange uses(instr);
  if (!uses.empty()) {
    console << " <= " << kInstrPrefix << user(uses.front())->id;
    while (uses.popFront(), !uses.empty())
      console << "," << kInstrPrefix << user(uses.front())->id;
  }
  return console << "\n";
}

void printInstr(Instr* instr) {
  printInstr(avmplus::AvmCore::getActiveCore()->console, instr);
}

PrintWriter& printPhi(PrintWriter& console, LabelInstr* label, int index) {
  Def* d = &label->params[index];
  for (LabelArgRange r(label, index); !r.empty(); r.popFront()) {
    if (def(r.front()) != d) { // ignore self. 
      printDef(def(r.front())); console << " ";
      printInstr(user(r.front())); console << "\n";
    }
  }
  printInstr(label);
  return console;
}

void printPhi(LabelInstr* label, int index) {
  printPhi(avmplus::AvmCore::getActiveCore()->console, label, index);
}

PrintWriter& printCompactInstr(PrintWriter& console, Instr* instr,
                               bool print_defs) {
  char namebuf[80];
  sprintf(namebuf, "    %s%-3d %-18.18s  ", kInstrPrefix, instr->id,
          name(instr));
  console << namebuf;
  print(console, instr);
  if (kind(instr) == HR_const) {
    console << typeName(cast<ConstantExpr>(instr)->value());
  } else {
    console << "(";
    for (ArrayRange<Use> u = useRange(instr); !u.empty(); u.popFront()) {
      printDef(console, def(u.front()));
      console << ":" << typeName(u.front()) << delim(u);
    }
    console << ") ";
    if (print_defs)
      for (ArrayRange<Def> d = defRange(instr); !d.empty(); d.popFront())
        //if (!isEffect(type(d.front())) && !isBottom(type(d.front())))
          console << typeName(d.front()) << delim(d);
  }
  return console << "\n";
}

void printCompactInstr(Instr* instr,
                       bool print_defs) {
  printCompactInstr(avmplus::AvmCore::getActiveCore()->console, instr, print_defs);
}


const char* label(BlockStartInstr* b) {
  return kind(b) == HR_label ? kJoinPrefix : kBlockPrefix;
}

BlockStartInstr* idom(BlockStartInstr* b, DominatorTree* doms) {
  return doms->hasIDom(b) ? doms->idom(b) : 0;
}

void listCfg(PrintWriter& console, InstrGraph* ir) {
#ifdef AVMPLUS_VERBOSE
  Allocator scratch;
  DominatorTree* doms = forwardDoms(scratch, ir);
  DominatorTree* rdoms = reverseDoms(scratch, ir);

  console << "=== IR size=" << ir->instr_count() << " ===\n";
  int instr_count = 0;
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    console << label(block) << block->blockid << ":";
    console << " dom={";
    for (BlockStartInstr* dom = idom(block, doms); dom != 0; dom = idom(dom, doms))
      console << label(dom) << dom->blockid << (doms->hasIDom(dom) ? "," : "");
    console << "} df={";
    for (SeqRange<BlockStartInstr*> r(doms->df(block)); !r.empty(); r.popFront())
      console << label(r.front()) << r.front()->blockid << delim(r);
    console << "} rdom={";
    for (BlockStartInstr* dom = idom(block, rdoms); dom != 0; dom = idom(dom, rdoms))
      console << label(dom) << dom->blockid << (rdoms->hasIDom(dom) ? "," : "");
    console << "} rdf={";
    for (SeqRange<BlockStartInstr*> r(rdoms->df(block)); !r.empty(); r.popFront())
      console << label(r.front()) << r.front()->blockid << delim(r);
    console << "}\n";
    if (kind(block) == HR_catchblock) {
      ((CatchBlockInstr*)block)->printCatchPreds();
    }

    for (InstrRange j(block); !j.empty(); j.popFront(), ++instr_count)
      printInstr(console, j.front());
  }
  console << "=== IR end size=" << ir->instr_count() <<
      " actual=" << instr_count << " ===\n\n";
#else
  (void) console;
  (void) ir;
#endif
}

void listInstr(PrintWriter& console, InstrGraph* ir) {
  for (EachBlock b(ir); !b.empty(); b.popFront()) {
    BlockStartInstr* block = b.front();
    console << kBlockPrefix << block->blockid << '\n';
    for (InstrRange i(block); !i.empty(); i.popFront())
      printInstr(console, i.front());
  }
  console << "\n";
}

void printAbcInstr(MethodInfo* m, PrintWriter& console, const uint8_t* pc) {
  PoolObject* pool = m->pool();
  AvmCore* core = pool->core;
  const uint8_t* code_pos = m->getMethodSignature()->abc_code_start();
  console << " " << int(pc - code_pos) << ":";
  const uint8_t* code_end = code_pos + 0xfffffff; // Hack code_end for now.

#ifdef AVMPLUS_VERBOSE
  // handle string literal formatting here so we can use single quotes
  if (*pc == OP_debugfile || *pc == OP_pushstring) {
    console << opcodeInfo[*pc++].name;
    uint32_t index = AvmCore::readU32(pc);
    console << " '" << pool->getString(index) << "'";
  } else {
    core->formatOpcode(console, pc, code_end, AbcOpcode(*pc), pc - code_pos,
                       pool);
  }
#else
  (void) core;
  (void) code_end;
#endif
  console << "\n";
}

void printDef(PrintWriter& out, const Def* d) {
  out << kDefPrefix << definerId(d);
  printDef(out, *d);
}

void printDef(const Def* d) {
  printDef(avmplus::AvmCore::getActiveCore()->console, d);
  fflush(stdout);
}

class PrintAdapter: public ShapeAdapter<PrintAdapter, void> {
 public:
  explicit PrintAdapter(PrintWriter& out) : out_(out) {
  }

  void do_default(Instr* /* instr */) {
  }

  void do_ArmInstr(ArmInstr* arm) {
    out_ << kInstrPrefix << arm->owner->id;
  }

  void do_GotoInstr(GotoInstr* go) {
    if (go->target)
      out_ << kInstrPrefix << go->target->id;
  }

  void do_SafepointInstr(SafepointInstr* instr) {
    out_ << instr->vpc << ":";
  }

  // DEOPT: We may want to print more info here.
  void do_DeoptSafepointInstr(DeoptSafepointInstr* instr) {
    out_ << instr->vpc << ":";
  }

  void do_SetlocalInstr(SetlocalInstr* i) {
    out_ << i->index;
  }

 private:
  PrintWriter& out_;
};

void print(PrintWriter& out, Instr* instr) {
  PrintAdapter p(out);
  do_shape(&p, instr);
}

class PrintDefAdapter: public ShapeAdapter<PrintDefAdapter, void> {
 public:
  PrintDefAdapter(PrintWriter& out, const Def& d) : out(out), d(d) {
  }

  void do_default(Instr*) {
  }

  void do_StartInstr(StartInstr* start) {
    if (&d == start->effect_out())
      out << "effect";
    else if (start->has_rest() && &d == start->rest_out())
      out << "rest";
    else
      out << "param" << (&d - start->data_param(0));
  }

  void do_LabelInstr(LabelInstr*) { doParam(); }
  void do_ArmInstr(ArmInstr*) { doParam(); }
  void do_UnaryStmt(UnaryStmt*) { doStmt(); }
  void do_BinaryStmt(BinaryStmt*) { doStmt(); }
  void do_CallStmt1(CallStmt1*) { doStmt(); }
  void do_CallStmt2(CallStmt2*) { doStmt(); }
  void do_CallStmt3(CallStmt3*) { doStmt(); }
  void do_CallStmt4(CallStmt4*) { doStmt(); }
  void do_NaryStmt0(NaryStmt0*) { doStmt(); }
  void do_NaryStmt2(NaryStmt2*) { doStmt(); }

  void do_Hasnext2Stmt(Hasnext2Stmt*) {
    static const char* names[] = { "effect", "value", "counter", "object" };
    out << names[pos(d)];
  }

 private:
  void doParam() {
    if (numDefs(definer(d)) > 1)
      out << "param" << pos(d);
  }

  void doStmt() {
    static const char* names[2] = { "effect", "value" };
    out << names[pos(d)];
  }

 private:
  PrintWriter& out;
  const Def& d;
};

void printDef(PrintWriter& out, const Def& d) {
  PrintDefAdapter a(out, d);
  do_shape(&a, definer(d));
}

void printDef(const Def& d) {
  printDef(avmplus::AvmCore::getActiveCore()->console, d);
}

const char* name(Instr* instr) {
  return name(*instr);
}

const char* name(Instr& instr) {
  return instr.info->name();
}

const char* typeName(const Type* t) {
  return t->name;
}

const char* typeName(const Def& value) {
  return typeName(type(value));
}

const char* typeName(const Use& value) {
  return typeName(def(value));
}

#ifdef NJ_VERBOSE

void printLirCfg(MethodInfo* method, Fragment* frag) {
  // For the control-flow graph :
  //   The list of instructions that we don't want to show explicit
  //   edges for are added to the 'ignore' set.
  Allocator alloc;
  InsSet ignore(alloc);
  FILE *f = openFile(method, "lir", ".gml");
  LirReader reader(frag->lastIns);
  CfgLister lister(&reader, alloc, CfgLister::CFG_BB);
  for (LIns* ins = lister.read(); !ins->isop(LIR_start); ins = lister.read()) {
  }
  lister.printGmlCfg(f, frag->lirbuf->printer, &ignore);
  fclose(f);
}


  void printString(Stringp string) {
    StUTF8String buf(string);
    fputs(buf.c_str(), stdout);
    // flush in case debugger output is interleaved with process output
    fflush(stdout);
  }

  void printAtom(Atom atom) {
    StringBuffer buf(avmplus::AvmCore::getActiveCore());
    buf.writeAtom(atom);
    fputs(buf.c_str(), stdout);
    // flush in case debugger output is interleaved with process output
    fflush(stdout);
  }
  
  void printMethod(MethodInfo* info) {
    String* name = info->getMethodName();
    if (!name) return;
    StUTF8String buf(info->getMethodName());
    fputs(buf.c_str(), stdout);
    // flush in case debugger output is interleaved with process output
    fflush(stdout);
  }



#endif

} // namespace avmplus
#endif // VMCFG_HALFMOON
