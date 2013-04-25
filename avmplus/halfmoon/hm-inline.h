/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * hm-inline.h
 *
 *  Created on: Apr 12, 2011
 *      Author: mzaleski
 */

#ifndef HM_INLINE_H_
#define HM_INLINE_H_

namespace halfmoon {

/// Inlining: visit the whole IR and inline nodes greedily.
///
/// \param cxt contains an early attempt at the "compilation context". We're going to need it to inline callees.
/// \param ir the instruction graph that we are to examine for inlinable callsites.
///
bool inlineGraph(Context *cxt, InstrGraph* ir, const char *title);

}

#endif /* OPT_INLINE_H_ */
