/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

/*
 * General value numbering notes.
 *
 * local: use a hashtable, include folding and identities.
 *
 * dominator-vn allows modifying the code as you scan, needs scoped table but
 * otherwise is similar to local-vn.
 *
 * unified hashtable: simple and nearly global, but doesnt handle loops,
 * and requires scheduling next.
 *
 * Taylor: vdcm beats lcm in power, compiler speed, and simplicity.
 * todo: http://dl.acm.org/citation.cfm?id=512536
 *
 * can we combine type propagation at the same time in sccvn or rpovn?
 * (yes, needs to be worklist algorithm).
 */

/**
 * Return true if we can swap this instruction's left/right operands,
 * possibly by using a different opcode.
 */
bool isSwappable(InstrKind k) {
  assert(InstrFactory::isBinaryExpr(k));
  switch (k) {
    case HR_addi:
    case HR_muli:
    case HR_ori:
    case HR_andi:
    case HR_xori:
    case HR_eqi:
    case HR_eqd:
    case HR_equi:
    case HR_lti:
    case HR_lei:
    case HR_gti:
    case HR_gei:
    case HR_ltd:
    case HR_led:
    case HR_gtd:
    case HR_ged:
    case HR_ltui:
    case HR_leui:
    case HR_gtui:
    case HR_geui:
      return true;
    default:
      return false;
  }
}

/** Return the opcode to use when swapping left/right operands */
InstrKind swap(InstrKind k) {
  assert(isSwappable(k));
  switch (k) {
    default: assert(false && "bad kind. fix isInvertable() or add case");
    case HR_addi:
    case HR_muli:
    case HR_ori:
    case HR_andi:
    case HR_xori:
    case HR_eqi:
    case HR_eqd:
    case HR_equi:
      return k;  // these are commutative
    case HR_lti: return HR_gti;
    case HR_lei: return HR_gei;
    case HR_gti: return HR_lti;
    case HR_gei: return HR_lei;
    case HR_ltd: return HR_gtd;
    case HR_led: return HR_ged;
    case HR_gtd: return HR_ltd;
    case HR_ged: return HR_led;
    case HR_ltui: return HR_gtui;
    case HR_leui: return HR_geui;
    case HR_gtui: return HR_ltui;
    case HR_geui: return HR_leui;
  }
}

const Type* fold(Lattice* l_, InstrKind k, const Type* t) {
  switch (k) {
    case HR_negi: return callstub(l_, t, Stubs::do_negi);
    case HR_negd: return callstub(l_, t, Stubs::do_negd);
    case HR_noti: return callstub(l_, t, Stubs::do_noti);
    case HR_u2i:  return callstub(l_, t, Stubs::do_u2i);
    case HR_i2u:  return callstub(l_, t, Stubs::do_i2u);
    case HR_d2b:  return callstub(l_, t, Stubs::do_d2b);
    default: return 0;
  }
}

const Type* fold(Lattice* l_, InstrKind k, const Type* x, const Type* y) {
  switch (k) {
    case HR_addi: return callstub(l_, x, y, Stubs::do_addi);
    case HR_subi: return callstub(l_, x, y, Stubs::do_subi);
    case HR_muli: return callstub(l_, x, y, Stubs::do_muli);
    default: return 0;
  }
}

class UnaryKey {
public:
  UnaryKey() {}
  UnaryKey(InstrKind k, Def* v) : kind(k), value(v) {}
  static size_t hash(UnaryKey& k) {
    return k.kind * defId(k.value);
  }
  bool operator!=(const UnaryKey& k) const { return !(*this == k); }
  bool operator==(const UnaryKey& k) const {
    return kind == k.kind && value == k.value;
  }
private:
  InstrKind kind;
  Def *value;
};

class UnaryStmtKey {
public:
  UnaryStmtKey() {}
  UnaryStmtKey(InstrKind k, int pos, Def* e, Def* v)
  : kind(k), pos(pos), effect(e), value(v) {}
  static size_t hash(UnaryStmtKey& k) {
    return (k.kind + 1) * (defId(k.value) + 1) + (k.pos + 1);
  }
  bool operator!=(const UnaryStmtKey& k) const { return !(*this == k); }
  bool operator==(const UnaryStmtKey& k) const {
    return kind == k.kind && pos == k.pos && value == k.value;
  }
private:
  InstrKind kind;
  int pos;
  Def *effect, *value;
};

class BinaryKey {
public:
  BinaryKey() {}
  BinaryKey(InstrKind k, Def* lhs, Def* rhs) : kind(k), lhs(lhs), rhs(rhs) {}
  static size_t hash(BinaryKey& k) {
    return k.kind * defId(k.lhs) * defId(k.rhs);
  }
  bool operator!=(const BinaryKey& k) const { return !(*this == k); }
  bool operator==(const BinaryKey& k) const {
    return kind == k.kind && lhs == k.lhs && rhs == k.rhs;
  }
private:
  InstrKind kind;
  Def *lhs, *rhs;
};

template<class K, class V>
struct ScopedTable {
  ScopedTable(Allocator& alloc, int size, ScopedTable<K,V>* parent)
  : map(alloc, size), parent(parent) {}
  HashMap<K,V,K,UseOperEqual<K> > map;
  ScopedTable<K,V>* parent;
};

class ExprTable {
public:
  ExprTable(Allocator& alloc, int size, ExprTable* parent = 0)
  : consts(alloc, 1 + size / 4, parent ? &parent->consts : 0),
    unary(alloc, 1 + size / 4, parent ? &parent->unary : 0),
    unary2(alloc, 1 + size / 4, parent ? &parent->unary2 : 0),
    binary(alloc, 1 + size / 4, parent ? &parent->binary : 0) {
  }
  Def* lookup(Def* d, const Type* type) {
    return lookup(d, &consts, TypeKey(type));
  }
  Def* lookup(Def* d, InstrKind kind, Def* val) {
    return lookup(d, &unary, UnaryKey(kind, val));
  }
  Def* lookup(Def* d, InstrKind kind, int pos, Def* effect, Def* value) {
    return lookup(d, &unary2, UnaryStmtKey(kind, pos, effect, value));
  }
  Def* lookup(Def* d, InstrKind kind, Def* lhs, Def* rhs) {
    return lookup(d, &binary, BinaryKey(kind, lhs, rhs));
  }
private:
  template <class K, class M>
  Def* lookup(Def* d, M* map, K k) {
    for (M* m = map; m; m = m->parent) {
      Def* t = m->map.get(k);
      if (t)
        return t;
    }
    map->map.put(k, d);
    return d;
  }
private:
  ScopedTable<TypeKey, Def*> consts;
  ScopedTable<UnaryKey, Def*> unary;
  ScopedTable<UnaryStmtKey, Def*> unary2;
  ScopedTable<BinaryKey, Def*> binary;
};

/** return true if this def is a constant. */
bool isConst(Def* x) {
  return isConst(type(x));
}

/**
 * Simplifier contains logic for simplifying expressions as much as
 * possible before doing a lookup in the given ExprTable.  Simplifications
 * include algebraic identities as well as constant folding.
 *
 * Many of the rules here duplicate ones found in IdentityAnalyzer. This
 * is a work in progress with the eventual goal of having one place to write
 * all the equations for algebraic identities.
 */
class Simplifier {
public:
  Simplifier(Lattice* lattice, ExprTable* ht)
  : l_(lattice), ht(ht) {}

  Def* simplify(Def* d, InstrKind k, Def* x);
  Def* simplify(Def* d, InstrKind k, Def* x0, Def* y0);
  Def* simplify(Def* d, InstrKind k, int pos, Def* effect, Def* value);

private:
  Lattice* l_;
  ExprTable* ht;
};

/** simplify a unary expression */
Def* Simplifier::simplify(Def* d, InstrKind k, Def* x) {
  assert(InstrFactory::isUnaryExpr(k));
  const Type* tx = type(x);
  if (isConst(tx)) {
    const Type* t = fold(l_, k, tx);
    if (t)
      return ht->lookup(d, t);
  }
  Def* t;
  switch (k) {
    case HR_negi:
      if (match(x, HR_negi, &t))
        return t;
      break;
    case HR_atom2scriptobject:
      if (match(x, HR_scriptobject2atom, &t))
        return t;
      break;
    case HR_castobject:
      if (match(x, l_->object_type[kTypeNullable]))
        return x;
      break;
    case HR_toboolean:
      if (match(x, l_->boolean_type))
        return x;
      break;
    case HR_doubletoint32:
      if (match(x, l_->int_type))
        return x;
      break;
    case HR_i2u:
      if (match(x, HR_u2i, &t))  // i2u(u2i(x)) = x
        return t;
      break;
    case HR_u2i:
      if (match(x, HR_i2u, &t))
        return t;
      break;
  }
  return ht->lookup(d, k, x);
}

/** simplify a binary expression */
Def* Simplifier::simplify(Def* d, InstrKind k, Def* x0, Def* y0) {
  assert(InstrFactory::isBinaryExpr(k));
  const Type* tx = type(x0);
  const Type* ty = type(y0);
  if (isConst(tx) && isConst(ty)) {
    const Type* c = fold(l_, k, tx, ty);
    if (c)
      return ht->lookup(d, c);
  }
  Def* x = x0;
  Def* y = y0;
  if (isSwappable(k)) {
    // normalize by putting constant on RHS, or sorting operands.
    if ((isConst(x0) && !isConst(y0)) || defId(x0) > defId(y0))
      (k = swap(k)), (x = y0), (y = x0);
  }
  Def* z;
  switch (k) {
    case HR_subi:
      if (x == y) return ht->lookup(d, l_->makeIntConst(0)); // x-x = 0
      if (match(y, 0)) return x; // x-0 = x
      if (match(y, HR_negi, &z))
        return simplify(d, HR_addi, x, z); // x-(-z) = x+z
      break;
    case HR_addi:
      if (match(y, 0)) return x; // x+0 = x
      if (match(x, HR_negi, &z))
        return simplify(d, HR_subi, y, z); // x+(-z) = x-z
      break;
    case HR_muli:
      if (match(y, 0))
        return y; // x * 0 = 0
      if (match(y, 1))
        return x; // x * 1 = x
      break;
    case HR_andi:
      if (match(y, 0))
        return y; // x & 0 = 0
      if (match(y, -1))
        return x; // x & -1 = x
      break;
  }
  return ht->lookup(d, k, x, y);
}

Def* Simplifier::simplify(Def* d, InstrKind k, int pos, Def* effect, Def* value) {
  assert(InstrFactory::isUnaryStmt(k));
  switch (k) {
    case HR_tonumber:
      if (match(value, l_->double_type))
        return pos ? value : effect;
      break;
    case HR_toint:
      if (match(value, l_->int_type))
        return pos ? value : effect;
      break;
    case HR_touint:
      if (match(value, l_->uint_type))
        return pos ? value : effect;
      break;
    case HR_caststring:
      if (match(value, l_->string_type[kTypeNotNull]))
        return pos ? value : effect;
      break;
    case HR_toprimitive:
      if (isPrimitive(type(value)))
        return pos ? value : effect;
      break;
    case HR_cknull:
    case HR_cknullobject:
      if (!isNullable(type(value)))
        return pos ? value : effect;
      break;
  }
  return ht->lookup(d, k, pos, effect, value);
}

/**
 * Dominator based value numbering.  Visit each block in reverse postorder,
 * simplifying instructions as we go.  We populate an expression table for
 * each block; each table is linked to the immediate dominator's table.
 * optimize-me (saves space, probably not time:)
 *   - use a unified scoped hashtable
 *   - do a preorderer traversal on the dom tree
 */
class DVN {
public:
  DVN(InstrGraph* ir) : ir_(ir) {}
  void pass();

private:
  void visit(ConstantExpr* instr, ExprTable* table);
  void visit(UnaryExpr* instr, Simplifier* s);
  void visit(UnaryStmt* instr, Simplifier* s);
  void visit(BinaryExpr* instr, Simplifier* s);
  void update(Def* d, Def* t);
private:
  InstrGraph* ir_;
};

void DVN::visit(ConstantExpr* instr, ExprTable* table) {
  Def* d = instr->value();
  update(d, table->lookup(d, type(d)));
}

void DVN::visit(UnaryExpr* instr, Simplifier* s) {
  Def* d = instr->value_out();
  update(d, s->simplify(d, kind(instr), def(instr->value_in())));
}

void DVN::visit(UnaryStmt* instr, Simplifier* s) {
  Def* effect_in = def(instr->effect_in());
  Def* value_in = def(instr->value_in());
  InstrKind k = kind(instr);
  Def* d = instr->effect_out();
  update(d, s->simplify(d, k, 0, effect_in, value_in));
  d = instr->value_out();
  update(d, s->simplify(d, k, 1, effect_in, value_in));
}

void DVN::visit(BinaryExpr* instr, Simplifier* s) {
  Def* d = instr->value_out();
  update(d, s->simplify(d, kind(instr), def(instr->lhs_in()),
                        def(instr->rhs_in())));
}

void DVN::update(Def* d, Def* t) {
  if (d != t) {
    if (enable_verbose)
      printf("domvn: replace %s d%d -> d%d\n", name(definer(d)), defId(d), defId(t));
    copyUses(d, t);
  }
}

void DVN::pass() {
  Allocator scratch;
  Allocator0 scratch0(scratch);
  DominatorTree* doms = forwardDoms(scratch, ir_);
  ExprTable** tables = new (scratch0) ExprTable*[ir_->block_count()];
  int table_size = 1 + ir_->instr_count() / ir_->block_count();
  for (EachBlock b(ir_); !b.empty(); ) {
    BlockStartInstr* block = b.popFront();
    ExprTable* idom_table = doms->hasIDom(block) ? tables[doms->idom(block)->blockid] : 0;
    assert((!doms->hasIDom(block) || idom_table) && "visited block before its dominator");
    ExprTable* table = new (scratch) ExprTable(scratch, table_size, idom_table);
    tables[block->blockid] = table;
    Simplifier s(&ir_->lattice, table);
    for (InstrRange i(block); !i.empty();) {
      Instr* instr = i.popFront();
      switch (shape(instr)) {
        case CONSTANTEXPR_SHAPE:
          visit(cast<ConstantExpr>(instr), table);
          break;
        case UNARYEXPR_SHAPE:
          visit(cast<UnaryExpr>(instr), &s);
          break;
//        case UNARYSTMT_SHAPE:
//          visit(cast<UnaryStmt>(instr), &s);
//          break;
        case BINARYEXPR_SHAPE:
          visit(cast<BinaryExpr>(instr), &s);
          break;
      }
    }
  }
}

/**
 * Hashtable-based dominator value numbering.
 */
void dominatorValueNumbering(Context*, InstrGraph* ir) {
  DVN dvn(ir);
  dvn.pass();
}

/**
 * Reverse postorder value numbering based on Cooper & Taylor.
 */
class RPOVN {
public:
  RPOVN(Allocator& alloc, Context* cxt, InstrGraph* ir)
  : alloc(alloc)
  , cxt(cxt)
  , ir(ir) {
    Allocator0 alloc0(alloc);
    vn_ = new (alloc0) Def*[ir->def_count()];
  }

  // do one pass over the code in reverse postorder, return true if
  // any vn[] entry changed.
  bool pass() {
    Allocator scratch;
    ExprTable table(scratch, ir->def_count());
    Simplifier s(&ir->lattice, &table);
    bool changed = false;
    for (EachBlock b(ir); !b.empty();)
      for (InstrRange i(b.popFront()); !i.empty();)
        changed |= visit(i.popFront(), &table, &s);
    return changed;
  }

  bool visit(Instr* instr, ExprTable* table, Simplifier* s) {
    switch (shape(instr)) {
      case CONSTANTEXPR_SHAPE:
        return visit(cast<ConstantExpr>(instr), table);
      case UNARYEXPR_SHAPE:
        return visit(cast<UnaryExpr>(instr), s);
      case UNARYSTMT_SHAPE:
        return visit(cast<UnaryStmt>(instr), s);
      case BINARYEXPR_SHAPE:
        return visit(cast<BinaryExpr>(instr), s);
      case LABELINSTR_SHAPE:
        return visit(cast<LabelInstr>(instr));
    }
    bool changed = false;
    for (ArrayRange<Def> r = defRange(instr); !r.empty();) {
      Def* d = &r.popFront();
      changed |= update(d, d);
    }
    return changed;
  }

  bool visit(LabelInstr* instr) {
    Def* defs = getDefs(instr);
    bool changed = false;
    for (int i = 0, n = numDefs(instr); i < n; ++i) {
      Def* d = &defs[i];
      Def* t = 0;
      for (LabelArgRange u(instr, i); !u.empty(); u.popFront()) {
        Def* v = vn(u.front());
        if (!v || v == t) continue;
        if (!t) {
          t = v;
        } else {
          t = d;
          break;
        }
      }
      assert(t);
      changed |= update(d, t);
    }
    return changed;
  }

  bool visit(ConstantExpr* instr, ExprTable* ht) {
    Def* d = instr->value();
    return update(d, ht->lookup(d, type(d)));
  }

  bool visit(UnaryExpr* instr, Simplifier* s) {
    Def* d = instr->value_out();
    return update(d, s->simplify(d, kind(instr), vn(instr->value_in())));
  }

  bool visit(UnaryStmt* instr, Simplifier* s) {
    Def* d0 = instr->effect_out();
    Def* d1 = instr->value_out();
    InstrKind k = kind(instr);
    Def* effect = vn(instr->effect_in());
    Def* value = vn(instr->value_in());
    return update(d0, s->simplify(d0, k, 0, effect, value)) |
           update(d1, s->simplify(d1, k, 1, effect, value));
  }

  bool visit(BinaryExpr* instr, Simplifier* s) {
    Def* d = instr->value_out();
    return update(d, s->simplify(d, kind(instr), vn(instr->lhs_in()),
                                 vn(instr->rhs_in())));
  }

  bool update(Def* d, Def* t) {
    assert(d && t);
    int dx = defId(d);
    if (vn_[dx] != t) {
      vn_[dx] = t;
      return true;
    }
    return false;
  }

  Def* vn(int i) {
    return vn_[i];
  }

private:
  Def* vn(const Use& u) { return vn_[defId(def(u))]; }

private:
  Allocator& alloc;
  Context* cxt;
  InstrGraph* ir;
  Def** vn_;
};

void globalValueNumbering(Context* cxt, InstrGraph* ir) {
  Allocator scratch;
  RPOVN rpovn(scratch, cxt, ir);
  int pass = 0;
  while (rpovn.pass()) {
    if (enable_verbose) {
      for (int i=0; i < ir->def_count(); i++) {
        const Def* t = rpovn.vn(i);
        if (t)
          printf("rpovn: %d vn[%d] = %d %s\n", pass, i, defId(t),
                 defId(t) != i ? "*" : "");
      }
      pass++;
      printf("\n");
    }
  }
  // do the transformation.
  for (AllInstrRange i(ir); !i.empty();) {
    Instr* instr = i.popFront();
    for (ArrayRange<Def> r = defRange(instr); !r.empty();) {
      Def* d = &r.popFront();
      Def* t = rpovn.vn(defId(d));
      if (t != d)
        copyUses(d, t);
    }
  }
}

} // namespace halfmoon
#endif // VMCFG_HALFMOON
