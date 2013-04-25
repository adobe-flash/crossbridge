/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace halfmoon {

/// Simple max function.
///
template<class T> T max(T a, T b) {
  return a > b ? a : b;
}

/// Simple min function.
///
template<class T> T min(T a, T b) {
  return a < b ? a : b;
}

/// Copy values from from_range to to_range.  Similar to STL copy(), except
/// that the ranges must be the same size, and no range is returned.
///
template<class FR, class TR>
inline void copyRange(FR from_range, TR to_range) {
  for (; !from_range.empty(); from_range.popFront(), to_range.popFront())
    to_range.front() = from_range.front();
  assert(to_range.empty());
}

/// Find value in range starting at the front.  Returns new range starting
/// with the found value.  Similar to STL find().
///
template<class R, class T>
inline R find(R r, T value) {
  for (; !r.empty(); r.popFront())
    if (r.front() == value)
      break;
  return r;
}

/// Count the number of elements in range.
///
template<class R>
inline int count(R r) {
  int n = 0;
  for (; !r.empty(); r.popFront())
    n++;
  return n;
}

/// Assign value to all elements in Range
///
template<class Range, class Value>
inline void fill(Range r, Value v) {
  for (; !r.empty(); r.popFront())
    r.front() = v;
}

} // namespace avmplus
