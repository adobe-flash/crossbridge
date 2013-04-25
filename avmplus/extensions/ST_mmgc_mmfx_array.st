// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category mmfx_array

%%methods
using namespace MMgc;

// There are different failure modes.
// 1) if you call mmfx_new_array() it can OOM:
//     -> it should OOM and shutdown
// 2) if you call mmfx_new_array_opts(canFail) and it OOMs:
//     -> it should return NULL and NOT OOM
// 3) if you call mmfx_new_aray_opts() without the canFail flag:
//     -> it should OOM and shutdown


%%explicit mmfx_new_array_oom
    // This will cause an OOM abort and exit, run with -memlimit 1024
    int* m_ints = mmfx_new_array(int, 1048576);
    mmfx_delete_array(m_ints);
    m_ints = NULL;
    %%verify false

%%explicit mmfx_new_array_opts_canFail
    // This should fail and return NULL, run with -memlimit 1024
    int* m_ints = mmfx_new_array_opt(int, 1048576, MMgc::kCanFail);
    %%verify m_ints == NULL
    mmfx_delete_array(m_ints);
    m_ints = NULL;

%%explicit mmfx_new_array_opts_oom
    // This will cause an OOM abort and exit, run with -memlimit 1024
    int* m_ints = mmfx_new_array_opt(int, 1048576, MMgc::kZero);
    mmfx_delete_array(m_ints);
    m_ints = NULL;
    %%verify false


