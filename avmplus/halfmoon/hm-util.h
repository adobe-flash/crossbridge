/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_UTIL_H_
#define HM_UTIL_H_

//
// Range-centric utility functions.  Nearly all iterator utility classes
// in Halfmoon use the range pattern.  A canonical loop using a range
// looks like this:
//
//    for (Range r(...); !r.empty(); r.popFront() { ... = r.front(); }
//
// and, popFront can return a value too, so you'll often see this:
//
//    for (Range r(...); !r.empty(); ) { ... = r.popFront(); }
//
// See: "On Iteration" by Andrei Alexandrescu
// http://www.informit.com/articles/printerfriendly.aspx?p=1407357
//

/**
 * MSVC-specific build error avoidance
 */
#ifdef _MSC_VER

// NOTE sorted by warning number
// TODO remove as time/code allow
#pragma warning(disable:4065)   // switch contains default but no cases
#pragma warning(disable:4200)   // zero-length array
#pragma warning(disable:4355)   // 'this' used in base member initializer list
#pragma warning(disable:4640)   // construction of local static object is not thread-safe

#endif

using nanojit::Allocator;
using nanojit::HashMap;
using nanojit::Seq;
using nanojit::SeqBuilder;

namespace halfmoon {
using avmplus::String;
using avmplus::ScriptObject;
using avmplus::Atom;
using avmplus::SST_atom;

/**
 * Range that accesses each active position in an ABC stack frame.
 */
template<class E>
class FrameRange {
public:
  FrameRange(E* frame, int stackp, int scopep, int stack_base) :
      frame(frame), stack_base(stack_base), stackp(stackp), scopep(scopep), i(0) {
  }

  bool empty() const {
    return i > stackp;
  }
  E& front() const {
    return frame[i];
  }
  void popFront() {
    i = (i != scopep) ? i + 1 : stack_base;
  }

private:
  E* const frame;
  const int stack_base;
  const int stackp;
  const int scopep;
  int i;
};

/**
 * Range that accesses each active position in an ABC stack frame.
 */
class FrameIndexRange {
public:
  FrameIndexRange(int stackp, int scopep, int stack_base) :
    stack_base(stack_base), stackp(stackp), scopep(scopep), i(0) {
  }

  bool empty() const {
    return i > stackp;
  }
  int front() const {
    return i;
  }
  void popFront() {
    i = (i != scopep) ? i + 1 : stack_base;
  }

private:
  const int stack_base;
  const int stackp;
  const int scopep;
  int i;
};

/// Return a FrameRange for frame, given the dimensions from state and signature.
///
template<class E>
FrameRange<E> range(E* frame, const FrameState* state,
                    MethodSignaturep signature) {
  int stack_base = signature->stack_base();
  int sp = stack_base + state->stackDepth - 1;
  int scopep = signature->scope_base() + state->scopeDepth - 1;
  return FrameRange<E>(frame, sp, scopep, stack_base);
}

}

// These extensions to nanojit allocators and containers could be moved
// into nanojit proper.
namespace halfmoon {

/** Wrapper for Allocator to enable an overloaded operator new that zeros */
struct Allocator0 {
  Allocator& alloc;
  Allocator0(Allocator& alloc) :
      alloc(alloc) {
  }
};

/**
 * Iterator for Seq<T>, allowing the following style:
 *
 * for (SeqRange<T> i(seq); !i.empty(); i.popFront()) {
 *   T item = i.front();
 *   ...
 * }
 */
template<class T>
class SeqRange {
public:
  SeqRange(Seq<T>* n) :
      n_(n) {
  }

  SeqRange(SeqBuilder<T> &seq) :
      n_(seq.get()) {
  }

  SeqRange(int i) :
      n_(0) {
    assert(i == 0);
    (void) i;
  }

public:
  // Range api
  bool empty() const {
    return n_ == NULL;
  }

  T& front() const {
    assert(!empty());
    return n_->head;
  }

  T& popFront() {
    T& t = front();
    n_ = n_->tail;
    return t;
  }

private:
  Seq<T> *n_;
};

} // end namespace nanojit

/** Global new overload enabling this pattern:  new (alloc, xtra) T(...) */
inline void* operator new(size_t size, Allocator &a, size_t count,
                          size_t elem_size) {
  return a.alloc(size + count * elem_size);
}

/** global operator new that returns a zero'd object */
inline void* operator new(size_t size, halfmoon::Allocator0& a) {
  void* p = a.alloc.alloc(size);
  VMPI_memset(p, 0, size);
  return p;
}

/** global operator new that returns an array of zero'd objects */
inline void* operator new[](size_t size, halfmoon::Allocator0& a) {
  void* p = a.alloc.alloc(size);
  VMPI_memset(p, 0, size);
  return p;
}

namespace halfmoon {

using avmplus::PrintWriter;
using nanojit::BitSet;

/**
 * A SeqStack implements a stack of elements of type T by maintaining
 * them in a linked list (Seq<T>), and keeping free list nodes in a pool
 * as the stack grows and shrinks.  List nodes are allocated from a
 * dedicated arena.
 */
template<class T> struct SeqStack {
  Allocator alloc;
  Seq<T>* stack;
  Seq<T>* free; // instr recycler

  SeqStack() :
      stack(0), free(0) {
  }

  void push(const T& elem) {
    Seq<T>* s = free;
    if (s) {
      free = s->tail;
      stack = new (s) Seq<T>(elem, stack); // Placement new.
    } else {
      stack = new (alloc) Seq<T>(elem, stack);
    }
  }

  T pop() {
    NanoAssert(!empty());
    Seq<T>* s = stack;
    stack = s->tail;
    s->tail = free;
    free = s;
    return s->head;
  }

  T& peek() const {
    NanoAssert(!empty());
    return stack->head;
  }

  bool empty() const {
    return !stack;
  }
};

/// cons head and tail using passed allocator, return new list
///
template<class T>
Seq<T>* cons(Allocator& alloc, T head, Seq<T>* tail) {
  return new (alloc) Seq<T>(head, tail);
}

/// Parse the given environment variable or return 0.  UNSAFE and only
/// for development.
///
inline int parseEnv(const char *name, int default_val) {
  const char *val = VMPI_getenv(name);
  return val ? atoi(val) : default_val;
}

/// ArrayRange implements the DoubleEndedRange pattern for an E[] Array.
///
template<class E> class ArrayRange {
public:
  ArrayRange(E* ptr, int count) :
      front_(ptr), back_(ptr + count) {
  }
  ArrayRange(const ArrayRange& other) :
      front_(other.front_), back_(other.back_) {
  }

public:
  bool empty() const {
    return front_ >= back_;
  }
  E& front() const {
    assert(!empty());
    return front_[0];
  }
  E& back() const {
    assert(!empty());
    return back_[-1];
  }
  E& popFront() {
    assert(!empty());
    return *front_++;
  }
  E& popBack() {
    assert(!empty());
    return *(--back_);
  }

private:
  E *front_, *back_;
};

/// A Chain range concatinates to unrelated ranges that have the same
/// element type.
///
template<class E, class R1, class R2> class Chain {
public:
  Chain(R1 r1, R2 r2) :
    r1(r1), r2(r2) {
  }

public:
  bool empty() const {
    return r1.empty() && r2.empty();
  }

  E& front() const {
    assert(!empty());
    return !r1.empty() ? r1.front() : r2.front();
  }

  void popFront() {
    assert(!empty());
    !r1.empty() ? r1.popFront() : r2.popFront();
  }

private:
  R1 r1;
  R2 r2;
};

} // namespace halfmoon

#endif // HM_UTIL_H_
