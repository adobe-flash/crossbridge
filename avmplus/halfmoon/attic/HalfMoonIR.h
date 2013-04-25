///
/// scratch - unused
///
class HalfMoonIR
{
  struct Def;
  struct Use;
  struct Branch;

  class Instr;
  class BlockInstr;

  /// an instance of a value is part of the one and only instruction that computes it as a result.
  /// Values are NOT held directly by the instructions consuming the value, rather, there are always use/def
  /// chain links inbetween (reword this).
  /// values are referred to instances of Use, use/def chain links, which are in turn referred to by
  /// the instructions using the value
  ///
  struct Def {
    Instr* owner;     // what instr defined this value (owner of this instance)
    Use* uses;        // list of uses of this value.
    Type* type;       // type of this value
  };

  /// instance of Use are use/def chain links.
  /// These link together the uses of each value, and are held by the actual consuming instruction.
  /// When an instruction holds a Use it indicates that the instruction is data dependent on the instruction that computes it as a result.
  ///
  struct Use {
    Def* def;       // which value is being used.
    Instr* owner;   // which instruction owns this use
    Use *next, *prev; // housekeeping for list of Uses of the same Def
  };

  /// base class of instructions. Most generic instruction does not have any results or uses..
  ///
  class Instr {
  public:
    const InstrInfo* info;
    int id;
    Instr *next, *prev; // pointers to next, prev instr (weak order)
  };

  /// example of an instruction that takes two inputs (instances of Use)
  /// and computes one result.
  ///
  class BinaryInstr: public Instr {
  public:
    Use args[2];
    Def result;
  };

  // statement version - adds control ordering
  class BinaryStmt : public Instr {
      Use args[2];      // data inputs: left operand, right operand
      Use next_stmt;    // control input - continuation, points to a Def me of another Stmt
      Def result;       // data result
      Def me;           // def site for use as continuation by predecessors
  };

  /// example of an iterator instruction with two inputs and three results.
  ///
  class Hasnext2Stmt : public Instr {
  public:
    Use args[2];        // object, counter
    Use next_stmt;
    Def results[3];     // hasmore, object, counter
    Def me;
  };

  // Legend:
  //    tail   -->  head
  //    source -->  sink
  //    Use    -->  Def
  //    GotoInstr -->  BlockInstr

  class BlockStmt : public Instr {
  public:
    Use next_stmt;
    Def me;
    Def params[0];  // off the end
  };

  // prev of any BlockStmt is the terminal branch stmt, e.g. IfStmt, JumpStmt
  Instr* term(BlockStmt* blockStmt) {
    return blockStmt->prev;
  }

  class IfStmt : public Instr {
  public:
    Use cond;           // data input - test condition
    Use nexts[2];       // nexts[0] is false branch. Elems point to Goto
    Def me;
  };

  class JumpStmt : public Instr {
    Use next[1];        // points to Goto
    Def me;
  };

  // next of any branch instr or GotoInstr is enclosing BlockStmt
  BlockStmt* block(Instr* instr) {
    return (BlockStmt*)instr->next;
  }

  // edge from branch to target block, carries args to block params
  class GotoInstr : public Instr {
  public:
    Def me;             // ONLY ONE USE, from branch stmt
    Use target;
    Use args[0];        // off the end
  };

  // any goto's prev pointer points to its owning branch (IfStmt, etc.)
  Instr* ownerBranch(GotoInstr* gotoInstr) {
    assert(gotoInstr->prev == gotoInstr->me.uses->owner);
    return gotoInstr->prev;
  }

  // ---

  // block:  N-in, 1-out  N is based on graph shape

  // stmt:   1-in, 1-out
  // branch: 1-in, K-out  K is based on kind of branch

  void deleteParam() {  // like deleting a column
    // SSA style:
    //    delete uses
    // CPS style:
    //    visit branches, remove 1 use from each branch
  }

  void deleteBranch() { // like deleting a row
    // SSA style:
    //    visit phis, remove 1 operand from each phi
    // CPS style:
    //    remove each of branch.args
  }

  // Operations
  Use*      uses(Instr*);       // return &instr->uses[0]
  Def*      defs(Instr*);       // return &instr->defs[0]
  Branch* branches(Instr*);     // return &instr->branches[0]

  int whichUse(Use* u)       { return u - uses(u->owner); }
  int whichDef(Def* d)       { return d - defs(d->owner); }
  //int whichWay(Downlink* d)  { return d - downlinks(d->owner); }

  void replaceValue(Def* v1, Def* v2) {
    (void) v1; (void) v2;
    // alluses of v1 get redirected to v2
    // refresh owners of modified instructions?
  }

};
