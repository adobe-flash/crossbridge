/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_MODELS_H_
#define HM_MODELS_H_

namespace halfmoon {

/// Adjust the IR so models are consistent everywhere.
///
void fixModels(Context*, InstrGraph* ir);

ModelKind promoteModelKind(const Type* t);

SlotStorageType type2sst(const Type*);

} // namespace halfmoon
#endif
