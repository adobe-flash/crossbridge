/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OPT_PROFILER_HH
#define OPT_PROFILER_HH

namespace halfmoon {
    class ProfiledInformation {
    public:
        ProfiledInformation();
        ~ProfiledInformation();
        void addBranchProbability(ArmInstr* arm_instr, double taken_probability);
        double getBranchProbability(ArmInstr* arm_instr);
        bool isHotterArm(ArmInstr* thinkHotArm, ArmInstr* thinkSlowArm);

    private:
        Allocator alloc_;
        HashMap<ArmInstr*, double> branch_probability_;
    };
}

#endif
