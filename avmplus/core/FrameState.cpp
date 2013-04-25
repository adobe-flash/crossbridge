/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    FrameState::FrameState(MethodSignaturep ms, MethodInfo* mi)
        : info(mi),
          wl_next(NULL), abc_pc(NULL),
          scopeDepth(0), stackDepth(0), withBase(-1),
          frameSize(ms->frame_size()),
          scopeBase(ms->scope_base()),
          stackBase(ms->stack_base()),
          targetOfBackwardsBranch(false),
          targetOfExceptionBranch(false),
          wl_pending(false)
    {
        locals = (FrameValue*)mmfx_alloc_opt(sizeof(FrameValue) * frameSize, MMgc::kZero);
    }

    FrameState::~FrameState()
    {
        mmfx_free( locals );
    }

    void FrameState::init(const FrameState* other)
    {
        scopeDepth = other->scopeDepth;
        stackDepth = other->stackDepth;
        withBase = other->withBase;
        targetOfBackwardsBranch = other->targetOfBackwardsBranch;
        targetOfExceptionBranch = other->targetOfExceptionBranch;
        VMPI_memcpy(locals, other->locals, frameSize * sizeof(FrameValue));
    }
}
