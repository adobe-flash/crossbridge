/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE size_t VTable::getExtraSize() const
{
    return traits->getExtraSize();
}

#ifndef VMCFG_AOT  // Avoid premature inlining for AOT; it prevents CSE
REALLY_INLINE MMgc::GC* VTable::gc() const
{
    return traits->core->GetGC();
}

REALLY_INLINE AvmCore* VTable::core() const
{
    return traits->core;
}

REALLY_INLINE Toplevel* VTable::toplevel() const
{
    return _toplevel;
}
#endif

} // namespace avmplus
