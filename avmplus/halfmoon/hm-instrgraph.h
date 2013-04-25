/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INSTR_GRAPH_H_
#define INSTR_GRAPH_H_

namespace halfmoon {

/// A Def is a value computed by an instruction.  Every def has a type.
/// For now, that's all a def has, however other facts might be interesting
/// to record here (for example how is the value used).  We do not conflate
/// Def with Type, nor with Instr.
///
class Def {
public:
  Def() :
    owner_(0), type_(0), uses_(0) {
  }
  explicit Def(Instr* owner) :
    owner_(owner), type_(0), uses_(0) {
  }
  Def(Instr* owner, const Type* t) :
    owner_(owner), type_(t), uses_(0) {
  }

  // Defs are only comparable by identity, the contents do not matter.
  bool operator==(const Def& other) const {
    assert(checkValid());
    return this == &other;
  }
  bool operator!=(const Def& other) const {
    assert(checkValid());
    return this != &other;
  }

  /// Copy one def to another only by copying the type, not the
  /// Use list or the owner.  todo: is this too subtle?
  ///
  void operator=(const Def& copy) {
    assert(checkValid());
    type_ = copy.type_; // don't change owner or uses.
  }

  /// Return true if there is at least one use.
  ///
  bool isUsed() const {
    return uses_ != 0;
  }

private:
  bool checkValid() const {
    // This poison check works on 32-bit and 64-bit as long as Allocator
    // gets memory from MMgc fixed alloc.
    return owner_ && uint32_t((uintptr_t)owner_) != uint32_t(MMgc::GCHeap::FXFreshPoison);
  }
  explicit Def(const Def& copy); // never implement.

private:
  Instr* const owner_; // The instruction that defines (computes) this def.
  const Type* type_; // The type of this def.
  Use* uses_; // Circularly linked set of uses of this def.

  friend class Use;
  friend class UseRange;
  friend Instr* definer(const Def&);
  friend const Type* type(const Def&);
  friend void setType(Def* d, const Type* t);
  friend void copyUses(Def*,Def*);
};

/// A Use is a member of an instruction and represents the "use"
/// endpoint of a data dependency on a Def in another instruction.
///
/// It's just a pointer to Def, with extra housekeeping to link to
/// other uses of the same Def.
///
class Use {
public:
  Use() :
    def_(0), owner_(0) {
  }

  explicit Use(Instr* instr) :
    def_(0), owner_(instr) {
  }

  Use(Instr* instr, Def* d) :
    owner_(instr) {
    set(d);
  }

  /// A Use can be assigned directly from a Def*; unlink this Use from
  /// the old def and link to the new def.
  ///
  void operator=(Def* d) {
    assert(checkValid());
    unlink();
    set(d);
  }

  void operator=(Def& d) {
    *this = &d;
  }

  /// Override the default assignment operator; when assigning one use to
  /// another, copy the underlying Def* and adjust links accordingly.
  ///
  void operator=(Use& u) {
    *this = u.def_; // operator=(u.def_)
  }

  /// Cast Use to bool returns true if def_ points to something,
  /// or false if def_ == 0.
  operator bool() const {
    return def_ != 0;
  }

private:
  /// Remove this Use from def's uses.
  ///
  void unlink() {
    if (!def_)
      return;
    // Remove this from def's uses list.
    if (next_ == this) {
      // this is the last element in the list.
      assert(prev_ == this && def_->uses_ == this);
      def_->uses_ = 0;
    } else {
      assert(next_ != this && prev_ != this);
      if (def_->uses_ == this)
        def_->uses_ = next_;
      prev_->next_ = next_;
      next_->prev_ = prev_;
    }
  }

  /// Set def_ to d and add this Use to d's uses if not-null.
  ///
  void set(Def* d) {
    def_ = d;
    if (!d)
      return;

    // Add this to head of d's uses list.
    if (d->uses_) {
      Use* N = next_ = d->uses_; // The node to the right (next)
      Use* P = prev_ = N->prev_; // The node to the left (prev)
      P->next_ = N->prev_ = this;
    } else {
      prev_ = next_ = d->uses_ = this;
    }

    d->uses_ = this;
  }

  bool checkValid() const {
    return owner_ && unsigned((uintptr_t)owner_) != 0xfafafafa;
  }

  explicit Use(const Use& other); // don't implement.

private:
  Def* def_; // The Def being used.
  Use *next_, *prev_; // Other uses of the same def.
  Instr* const owner_; // The Instr this use is inside of.

  friend class Def;
  friend class UseRange;
  friend Instr* user(const Use&);
  friend Def* def(const Use&);
  friend void copyUses(Def*,Def*);
};

/// Range over the uses of a single def.
///
class UseRange {
public:
  explicit UseRange(const Def& d) :
    def(&d) {
    Use* N = use = d.uses_;
    next = (N && N->next_ != N) ? N->next_ : 0;
  }

  bool empty() const {
    return !use;
  }

  Use& popFront() {
    Use& u = front();
    Use* N = use = next;

    // If N->next == def->uses_, then use is pointing to the last node.
    // We'll be finished when the next popFront() occurs.  def->uses_ can
    // change during iteration, so remember we're done by setting next=null.
    next = (N && N->next_ != def->uses_) ? N->next_ : 0;
    return u;
  }

  Use& front() const {
    assert(!empty());
    return *use;
  }

private:
  UseRange() :
    def(0), use(0), next(0) {
  }
  friend class AllUsesRange;

private:
  const Def* const def; // The def whose list we're iterating.
  Use* use; // The current use.

  // the next node, or null if we're on the last use.
  Use* next;
};

/// InstrInfo instances describe the layout of a concrete Instr,
/// and provide the virtual operations for the instr.
///
class InstrInfo {
public:
  /// Integrate given quantity and offset arguments
  /// with canonical shape info
  InstrInfo(
    int _num_uses, int _num_defs,
    int _uses_off, int _defs_off,
    InstrKind kind,
    const Type** insig,
    const Type** outsig,
    InstrGraph* ir = NULL) :
      kind_(kind),
      insig_(insig),
      outsig_(outsig),
      shape_(instr_attrs[kind].shape),
      num_uses(_num_uses),
      num_defs(_num_defs),
      uses_off(_uses_off),
      defs_off(_defs_off),
      ir(ir) {
    checkValid();
  }

  InstrInfo(const InstrInfo& other) {
    kind_    = other.kind_;
    insig_   = other.insig_;
    shape_   = other.shape_;
    num_uses = other.num_uses;
    num_defs = other.num_defs;
    uses_off = other.uses_off;
    defs_off = other.defs_off;
  }

  /// Return printable instruction name
  const char* name() const {
    return instr_attrs[kind_].name;
  }

private:
  /// check validity of layout data against generated shape
  ///
  void checkValid() {
    assert(kind_ >= 0 && kind_ < HR_MAX);

    const ShapeRep& rep(shape_reps[shape_]);
    int var = rep.vararg;

    // num_uses == -1 indicates non-inline storage of Uses
    if (num_uses >= 0) {
      if (var == kVarIn) {
        assert(num_uses >= rep.num_uses &&
            "number of inputs less than fixed minimum");
      } else {
        assert(num_uses == rep.num_uses &&
            "fixed number of inputs differs from shape");
      }
    }

    // num_defs == -1 indicates non-inline storage of Defs
    if (num_defs >= 0) {
      if (var == kVarOut) {
        assert(num_defs >= rep.num_defs &&
            "number of outputs less than fixed minimum");
      } else {
        assert(num_defs == rep.num_defs &&
            "fixed number of outputs differs from shape");
      }
    }
  }

private:
  InstrKind kind_; // The HR opcode.
  const Type** insig_; // input signature
  const Type** outsig_; // output signature

public:
  InstrShape shape_; // key to representation info

  int num_uses;
  int num_defs;
  int uses_off;
  int defs_off;

private:
  InstrGraph* ir; // subgraph for this instruction, or null.

  friend bool hasSubgraph(const Instr*);
  friend InstrGraph* subgraph(const Instr*);
  friend bool hasInputSignature(const Instr*);
  friend bool hasOutputSignature(const Instr*);
  friend const Type** inputSignature(const Instr*);
  friend const Type** outputSignature(const Instr*);
  friend InstrKind kind(const Instr&);
};

/// Instr is the basic instr type for the graph.  Every Instr has:
/// - a InstrInfo that describes its layout (its type descriptor)
/// - a sequential id used as a simple key for various traversals
/// - next_ and prev_ pointers that embed the Instr in a doubly-linked list
///
/// Attributes shared among many instructions of a single kind are factored
/// into the corresponding InstrInfo class.
///
class Instr {
public:
  explicit Instr(const InstrInfo* info) :
    info(info), id(-1), next_(0), prev_(0) {
  }

private:
  void addAfter(Instr* i) {
    assert(!isLinked());
    assert(i && i->isLinked());
    prev_ = i;
    Instr* next = next_ = i->next_;
    next->prev_ = i->next_ = this;
  }

  void addBefore(Instr* i) {
    assert(!isLinked());
    assert(i && i->isLinked());
    next_ = i;
    Instr* prev = prev_ = i->prev_;
    i->prev_ = prev->next_ = this;
  }

  bool isAlone() const {
    assert(isLinked());
    return next_ == this;
  }

  bool isLinked() const {
    assert(checkLinks());
    return next_ != 0;
  }

  bool checkLinks() const {
    return (!next_ && !prev_) ||
      ((next_ && next_->prev_ == this) && (prev_ && prev_->next_ == this));
  }

public:
  const InstrInfo* info; // todo: encapsulate access to this, especially assignment.
  int id; // Sequential id for this Instr.

private:
  Instr *next_, *prev_; // Links to other instructions in sequence.

private:
  friend class InstrGraph;
  friend class InstrGraphBuilder;
  friend class InstrRange;
  friend class Copier;
};

/// -------------------------------------------------------------------------------
///
/// predicates and accessors on Instr
///
///

/// Return true iff this instruction is a block start delimiter.
///
bool isBlockStart(const Instr*);

/// Return true iff this instruction is a block end delimiter.
///
bool isBlockEnd(const Instr*);

/// Return true iff this is an if or branch instruction.
///
bool isCond(const Instr*);

/// True if an instruction has root Defs, i.e. Defs whose types
/// are axiomatic. Defs introduced by ConstantExprs and
/// predecessor-free block starts have this property.
///
bool hasRootDefs(const Instr*);

/// true if end/start are a pair of related delimters,
/// i.e. if end is predecessor and start is successor.
///
bool isDelimPair(const Instr* end, const Instr* start);

/// if CondInstr's selector is constant, return the
/// selected arm, otherwise null.
///
ArmInstr* getConstArm(CondInstr* cond);
ArmInstr* getConstArm(CondInstr* cond, const Type* selector);

///
/// num{Defs,Uses,Args}
///

/// Return the total number of Defs in instr.
///
int numDefs(const Instr* instr);

/// Return the total number of Uses in instr.
///
int numUses(const Instr*);

/// Return the number of arg Uses in a BlockEndInstr.
///
int numArgs(const BlockEndInstr*);

///
/// contiguous get{Defs,Uses,Args}
///

/// Return a pointer to a contiguous array of all instr's Defs.
///
Def* getDefs(const Instr*);

/// Return a pointer to a contiguous array of all instr's Uses.
///
Use* getUses(const Instr*);

/// Return an allocated array of the types of the uses of an instr.
///
const Type** getUseTypes(Allocator& alloc, Instr* instr);

/// Return a pointer to a contiguous array of all BlockEndInstr's arg Uses.
///
Use* getArgs(const BlockEndInstr*);

/// Copy uses from one def to another.
///
void copyUses(Def* from, Def* to);

/// Redirect uses of old_instr's defs to new_instr's defs.  Both instructions
/// must have the same number of defs.
///
void copyAllUses(Instr* old_instr, Instr* new_instr);

/// return the Shape associated with this instruction
///
inline InstrShape shape(const Instr* instr) {
  return instr->info->shape_;
}

/// Return instr's opcode.
///
inline InstrKind kind(const Instr& instr) {
  return instr.info->kind_;
}

/// Return instr's opcode.
///
inline InstrKind kind(const Instr* instr) {
  return kind(*instr);
}

/// Set the type of each Def to UN.
///
void resetTypes(Instr*);

/// Set the type of d to t, then return true if the d's type changed.
///
inline void setType(Def* d, const Type* t) {
  assert(t && "never set Def.type_ to null!");
  d->type_ = t;
}

/// Return the Def pointed to by u.
///
inline Def* def(const Use& u) {
  assert(u.def_);
  return u.def_;
}

/// Return def's type.
///
inline const Type* type(const Def& def) {
  assert(def.type_ && "type_ must never be null");
  return def.type_;
}

/// Return def's type; (overloaded for Def*).
///
inline const Type* type(const Def* def) {
  return type(*def);
}

/// Return the type of the def pointed to by use.
///
inline const Type* type(const Use& use) {
  return type(def(use));
}

/// Return the instruction that defines d.
///
inline Instr* definer(const Def& d) {
  return d.owner_;
}

/// Return the instruction that defines d (overloaded for Def*).
///
inline Instr* definer(const Def* d) {
  return definer(*d);
}

/// Return the instruction that defines the value being used by u.
///
inline Instr* definer(const Use& u) {
  return definer(def(u));
}

inline int definerId(const Def& d) {
  return definer(d)->id;
}

inline int definerId(const Use& u) {
  return definer(u)->id;
}

inline int definerId(const Def* d) {
  return definer(d)->id;
}

/// Return the instruction that owns use.
///
inline Instr* user(const Use& use) {
  return use.owner_;
}

/// Return definer(d)'s opcode.
///
inline InstrKind kind(const Def& d) {
  return kind(definer(d));
}

/// Return definer(d)'s opcode.
///
inline InstrKind kind(const Def* d) {
  return kind(definer(d));
}

/// Return definer(def(u))'s opcode.
///
inline InstrKind kind(const Use& u) {
  return kind(def(u));
}

/// Return the position of d within the array of Defs of the definer.
/// The position P is in the range 0 <= P < numDefs(definer(d)).
///
inline int pos(const Def& d) {
  ptrdiff_t f = &d - getDefs(definer(d));
  assert(f >= 0 && f < numDefs(definer(d)));
  return int(f);
}

inline int pos(const Def* d) {
  return pos(*d);
}

inline int pos(const Use& u) {
  ptrdiff_t k = &u - getUses(user(u));
  assert(k >= 0 && k < numUses(user(u)));
  return int(k);
}

/// Return the uniqe integer id of this def.
///
inline int defId(const Def* d) {
  return definer(d)->id + pos(*d);
}

/// return the position of an Arm within its owner's array
///
int pos(ArmInstr* arm);

/// True if there is an InstrGraph for the implementation of i.  In other
/// words, return true if i is "fat".
///
inline bool hasSubgraph(const Instr* i) {
  return i->info->ir != NULL;
}

/// Return i's implementation InstrGraph.
///
inline InstrGraph* subgraph(const Instr* i) {
  return i->info->ir;
}

/// True if this instruction has an
/// input type signature.
///
inline bool hasInputSignature(const Instr* i) {
  return i->info->insig_ != NULL;
}

/// Return i's input signature.
///
inline const Type** inputSignature(const Instr* i) {
  assert(hasInputSignature(i));
  return i->info->insig_;
}

/// True if this instruction has an
/// output type signature.
///
inline bool hasOutputSignature(const Instr* i) {
  return i->info->outsig_ != NULL;
}

/// Return i's output signature.
///
inline const Type** outputSignature(const Instr* i) {
  assert(hasOutputSignature(i));
  return i->info->outsig_;
}

/// Safely downcast to INSTR*.
/// Note: INSTR must be a subclass of Instr with an
/// invariant shape.
///
///
template<class INSTR>
inline INSTR* cast(Instr* instr) {
  assert(!instr || shape(instr) == INSTR::shape);
  return (INSTR*)instr;
}

/// Safely downcast to const INSTR*
/// Note: INSTR must be a subclass of Instr with an
/// invariant shape.
///
template<class INSTR>
inline const INSTR* cast(const Instr* instr) {
  assert(!instr || shape(instr) == INSTR::shape);
  return (const INSTR*)instr;
}

/** true if x is a constant int and equal to c */
bool match(Def* x, int c);

/** true if definer(x) is unary opcode k.  If so return the input def */
bool match(Def* x, InstrKind k, Def** val);

/** true if definer(def(x)) is unary opcode k.  If so return the input def */
bool match(const Use& x, InstrKind k, Def** val);

/** true if type(d) is a subtype of t */
bool match(Def* d, const Type* t);

/// An InstrGraphBuilder maintains state useful while building an InstrGraph.
///
class InstrGraphBuilder {
public:
  InstrGraphBuilder(InstrGraph* ir, InstrFactory* factory, Instr* pos = 0);

  /// Add instr to this graph by assigning it an id, computing its type,
  /// and appending it to the current block.  If instr is a block start,
  /// then a new basic block is established and the insertion point is
  /// set to instr.
  Instr* addInstr(Instr* instr);

  /// Establish a new insertion point for new instructions.  They will be
  /// linked before instr.  If instr is a block start, new instructions are
  /// appended to the end of the block.
  void setPos(Instr* instr);

  /// Create a constant of the given type.  The value and type of
  /// the constant are represented by Type.
  ///
  Def* addConst(const Type* t);

  /// Compute the types of i's defs.
  ///
  void computeType(Instr* i);

  InstrFactory& factory() {
    return factory_;
  }

private:
  PrintWriter& console() const;

private:
  InstrGraph* const ir_;
  InstrFactory& factory_;
  Instr* pos_; // Insertion point for new instructions.
  HashMap<TypeKey, Def*, TypeKey, UseOperEqual<TypeKey> > constants_;
};

/// An InstrRange is a double-ended range for traversing lists of instructions
/// via their internal prev/next pointers.
///
class InstrRange {
public:
  /// Initialize the range with a specific start and back_.  They must be connected.
  ///
  InstrRange(Instr* start, Instr* end) :
    front_(start), back_(end) {
  }

  /// Initialize the range starting with start, ending with start->prev.
  ///
  explicit InstrRange(Instr* start) :
    front_(start), back_(start ? start->prev_ : 0) {
  }

  bool empty() const {
    return !front_;
  }

  Instr* front() const {
    assert(!empty());
    return front_;
  }

  Instr* back() const {
    assert(!empty());
    return back_;
  }

  Instr* popFront() {
    assert(!empty());
    Instr* F = front_;
    front_ = (F == back_) ? (back_ = 0) : F->next_;
    return F;
  }

  Instr* popBack() {
    assert(!empty());
    Instr* B = back_;
    back_ = (B == front_) ? (front_ = 0) : B->prev_;
    return B;
  }

  void unlinkFront() {
    Instr* instr = front();
    popFront();
    unlinkInstr(instr);
  }

  void unlinkBack() {
    Instr* instr = back();
    popBack();
    unlinkInstr(instr);
  }

private:
  /// Remove instr from whatever list it is in, without regard for whether
  /// it is the last one or not.
  ///
  static void unlinkInstr(Instr* instr) {
    assert(instr->isLinked());
    instr->next_->prev_ = instr->prev_;
    instr->prev_->next_ = instr->next_;
    instr->next_ = instr->prev_ = 0;
  }

private:
  Instr *front_, *back_;

  friend class InstrGraph;
};

/// A InstrGraph is a container for a graph of instructions.  We have the
/// start and stop instructions, as roots; all other instructions are reachable
/// from them.
///
class InstrGraph {
public:
  InstrGraph(InstrFactory*, InfoManager*);

  /// Return size of a container that can index using instr->id.
  ///
  int size() const {
    return def_count_;
  }

  int block_count() const {
    return block_count_;
  }

  /// Assign an id to instr, which must be a freshly new'd instruction.
  ///
  void assignId(Instr* instr) {
    assert(instr->id == -1);
    instr->id = def_count_;
    int num_defs = numDefs(instr);
    def_count_ += num_defs > 0 ? num_defs : 1;
    ++instr_count_;
  }

  /// Assign a new sequential block id to this block.
  ///
  void assignBlockId(BlockStartInstr* block);

  /// Add instr to this graph after pos, in the block occupied by pos.
  ///
  Instr* addInstrAfter(Instr* pos, Instr* instr);
  Instr* addInstrBefore(Instr* pos, Instr* instr);

  /// Add block start instr to begin a new block.
  ///
  Instr* addBlock(BlockStartInstr* instr);

  /// Create a new block starting with label, containing all of the
  /// instructions from instr's block, including the block end instr.
  Instr* addBlockAfter(BlockStartInstr* instr, LabelInstr* label);

  /// Return a pointer to the last statement in the graph, which must
  /// be the single control predecessor of this->back_.  Used for subgraphs.
  StopInstr* returnStmt() const;
  StopInstr* throwStmt() const;
  InstrRange returnBlock() const;

  /// Search for the block that encloses the given instruction.
  ///
  static InstrRange blockRange(Instr* instr) {
    return InstrRange((Instr*)findBlockStart(instr));
  }

  /// Remove instr from whatever list it is in, without regard for whether
  /// it is the last one or not.
  static void unlinkInstr(Instr* instr) {
    InstrRange::unlinkInstr(instr);
  }

  /// Link replacement in wherever instr is, then remove instr.
  ///
  void replaceInstr(Instr* instr, Instr* replacement) {
    addInstrBefore(instr, replacement);
    unlinkInstr(instr);
  }

  /// Exchange the arms of an If instruction.
  ///
  static void swapArms(IfInstr* instr);

  /// True iff instr is linked into a list of instructions.
  ///
  static bool isLinked(Instr* instr) {
    return instr->isLinked();
  }

  /// Return true if block is empty other than the block-end instruction.
  ///
  static bool isEmptyBlock(BlockStartInstr* block);

  /// Given a block instruction (isBlockStart() returns true),
  /// return the corresponding back_ (jump, branch, etc) instruction.
  ///
  static BlockEndInstr* blockEnd(BlockStartInstr* block);

  /// Return true if this block has a block-end.
  ///
  static bool hasBlockEnd(BlockStartInstr* block);

  /// Find the block owner, given a branch instruction.
  ///
  static BlockStartInstr* blockStart(BlockEndInstr* branch);

  void inlineBlocks(Instr* callsite, InstrRange start_block,
                    InstrRange return_block);

  /// Join two basic blocks together, removing the inbetween block end
  /// and block start instructions.
  ///
  void joinBlocks(BlockEndInstr* end, BlockStartInstr* start);

  /// Replace instruction pos with the list of instructions given by list.
  /// before:  P<=>pos<=>N     list
  /// after:   P<=>list<=>N
  ///
  /// \param pos postion where the instructions are to be inserted.
  /// \param list Infirst head of circular list of instructions (fragment) to be inserted.
  ///
  void replaceInstr(Instr* pos, const InstrRange& list);

  /// Replace pos with two lists of instructions.
  /// \param pos instruction in a block to replace (ie it will be removed)
  /// \param before_list is a list of instructions to insert before pos.
  /// \param after_list is a list of instructions to insert after pos
  ///
  /// This transformation is for the situation of replacing one instruction
  /// with a series of blocks involving control flow.  The only two blocks
  /// that matter are the first and last ones.
  ///
  void replaceInstr2(Instr* pos, const InstrRange& before_list,
      const InstrRange& after_list);

  /// Link instr just before pos.
  ///
  static void linkBefore(Instr* pos, Instr* instr) {
    assert(pos && pos->isLinked());
    assert(instr && !instr->isLinked());
    assert(!isBlockStart(instr));
    instr->addBefore(pos);
  }

  /// Link instr just after pos.
  ///
  static void linkAfter(Instr* pos, Instr* instr) {
    assert(pos && pos->isLinked());
    assert(instr && !instr->isLinked());
    assert(!isBlockEnd(pos));
    instr->addAfter(pos);
  }

  // Accessors.

  InfoManager* infos() {
    return infos_;
  }
  Allocator& alloc() {
    return alloc_;
  }
  int def_count() const {
    return def_count_;
  }
  int instr_count() const {
    return instr_count_;
  }

private:
  /// True iff list1 and list2 are in separate lists.
  ///
  static bool checkDisjoint(Instr* list1, Instr* list2);

  /// True if instr is not in list
  ///
  static bool checkDisjoint(Instr* instr, const InstrRange& list);

  /// Split a list of instructions into two lists.
  /// The first list begins at first, the second list begins with pos.
  ///
  /// \param pos instruction in a block to split at.  Second list will begin
  /// with pos.
  /// \param first is a list of instructions to split.
  ///
  void splitList(const InstrRange &list, Instr *pos);

  /// Link a list in before pos
  /// \param pos instruction that list will be inserted before
  /// \param list to insert
  ///
  void insertList(const InstrRange& list, Instr *pos);

  /// Link a list in past pos.
  /// \param pos instruction that list will be appended to
  /// \param list to append
  ///
  void appendList(Instr* pos, const InstrRange& list);

  /** Iteratively find the block that instr is a member of. */
  static BlockStartInstr* findBlockStart(Instr* instr);

public:
  /** Iteratively find the block end for the block that instr is in. */
  static BlockEndInstr* findBlockEnd(Instr* instr);

public:
  // fixme: make private
  Lattice& lattice;

  StartInstr* begin;
  StopInstr* end; // return
  StopInstr* exit; // throw

private:
  InfoManager* infos_;
  Allocator& alloc_;
  int instr_count_;
  int block_count_;
  int def_count_;
};

inline ArrayRange<Use> useRange(Instr* instr) {
  return ArrayRange<Use> (getUses(instr), numUses(instr));
}

inline ArrayRange<Use> argRange(BlockEndInstr* instr) {
  return ArrayRange<Use> (getArgs(instr), numArgs(instr));
}

inline ArrayRange<Def> defRange(Instr* instr) {
  return ArrayRange<Def> (getDefs(instr), numDefs(instr));
}

/// Remove goto->label and use->def edges that connect unlinked instructions
/// to linked ones.  Such edges are common byproducts of other
/// transformations.
void pruneGraph(InstrGraph* ir);

/// return a Range over the types of a signature.
/// signatures ending in a vararg produce an 'infinite' range.
///
class SigRange {
  const Type** sig;
  int len;
  int vararg;
  int pos;

public:
  SigRange(const Type** sig, int len, int vararg = -1) :
    sig(sig),
    len(len),
    vararg(vararg >= 0 ? vararg : len),
    pos(0) {
  }

  bool empty() const {
    return pos == len;
  }

  const Type* front() const {
    assert(!empty());
    return sig[pos];
  }

  void popFront() {
    assert(!empty());
    if (pos < vararg)
      pos++;
  }
};

/**
 * return a Range over an instruction's input signature.  Instructions with
 * no inputs (e.g. arm, label, start) return an empty range.
 */
inline SigRange inputSigRange(Instr* instr) {
  const ShapeRep& rep = shape_reps[shape(instr)];
  int vararg = rep.vararg == kVarIn ? rep.num_uses : -1;
  return hasInputSignature(instr) ?
    SigRange(inputSignature(instr), numUses(instr), vararg) :
    SigRange(0, 0, 0);
}

/**
 * return a Range over an instruction's output signature.  Instructions with
 * no outputs (e.g. if, switch, goto, return, throw) return an empty range.
 */
inline SigRange outputSigRange(Instr* instr) {
  const ShapeRep& rep = shape_reps[shape(instr)];
  int vardef = rep.vararg == kVarOut ? rep.num_defs : -1;
  return hasOutputSignature(instr) ?
      SigRange(outputSignature(instr), numDefs(instr), vardef) :
      SigRange(0, 0, 0);
}


/// return a Range over the arms of the given CondInstr
///
ArrayRange<ArmInstr*> armRange(CondInstr* instr);

/// Return a arm range sorted by probability that the branch is taken
///
ArrayRange<ArmInstr> hotArmRange(CondInstr* instr);

/// Iterate through all the uses of all the defs of an instruction.  This
/// iterator is a composition of ArrayRange<Def> and UseRange, equivalent to:
///
///   for (ArrayRange<Def> d = defRange(instr); d; ++d)
///     for (UseRange u(d); u; ++u)
///
class AllUsesRange {
public:
  explicit AllUsesRange(Instr* instr) :
    d(defRange(instr)), u() {
    next();
  }

  bool empty() const {
    return d.empty() || u.empty();
  }

  void popFront() {
    assert(!empty());
    if (u.popFront(), u.empty()) {
      d.popFront(); // Move to next d, then re-construct u if we aren't done.
      next();
    }
  }

  Use& front() const {
    assert(!empty());
    return u.front();
  }

private:
  // Skip ahead until we're pointing to a valid use, or until we exaust all
  // the defs.
  void next() {
    for (; !d.empty(); d.popFront()) {
      new (&u) UseRange(d.front()); // placement new.
      if (!u.empty())
        return;
    }
  }

private:
  ArrayRange<Def> d;
  UseRange u;
};

/// EachBlock iterates through the blocks of an InstrGraph,
/// in postorder, or reverse postorder, depending on ctor arg
/// (default is reverse postorder) over successors, starting
/// from ir->begin.
///
/// Cheese alert: this is implemented by running a depth first search
/// in the constructor, building a list, then simply wrapping a SeqRange
/// that visits each block instruction.
///
/// It probably would be better to maintain a canonical reverse-postorder
/// list of blocks in InstrGraph to avoid redoing the DFS over and over.
///
class EachBlock {
public:
  explicit EachBlock(InstrGraph* ir, bool reverse = true);

public:
  // Range api
  bool empty() const {
    return iter.empty();
  }
  BlockStartInstr* front() const {
    assert(!empty());
    return iter.front();
  }
  BlockStartInstr* popFront() {
    assert(!empty());
    return iter.popFront();
  }

private:
  void dfs(BlockStartInstr* block, bool reverse);
  explicit EachBlock(const EachBlock&); // do not implement.

private:
  Allocator scratch;
  InstrGraph* ir;
  BitSet visited; // one bit per block, indexed by blockid
  SeqBuilder<BlockStartInstr*> blocks;
  SeqRange<BlockStartInstr*> iter;
};

/// Visit all instructions in the CFG.  Blocks are visited in reverse postorder
/// and instructions within a block are visited in forwards order.
class AllInstrRange {
public:
  AllInstrRange(InstrGraph* ir)
  : b(ir), i(0) {
    if (!b.empty())
      i = InstrRange((Instr*)b.popFront());
  }
  bool empty() const {
    return i.empty();
  }
  Instr* front() const {
    return i.front();
  }
  Instr* popFront() {
    Instr* instr = i.popFront();
    while (i.empty() && !b.empty())
      i = InstrRange((Instr*)b.popFront());
    return instr;
  }
private:
  EachBlock b;
  InstrRange i;
};

/// Postorder block range
/// TODO: reconcile class naming with EachBlock
///
class PostorderBlockRange : public EachBlock {
public:
  explicit PostorderBlockRange(InstrGraph* ir) :
      EachBlock(ir, false) {
  }
};

} // namespace halfmoon

#endif // INSTR_GRAPH_H_
