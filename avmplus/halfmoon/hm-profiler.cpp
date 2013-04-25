/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

ProfiledInformation::ProfiledInformation() :
    branch_probability_(alloc_)
{

}

ProfiledInformation::~ProfiledInformation() {
}

/// Maps an arm instr to the probability that the arm is actually taken.
///
void ProfiledInformation::addBranchProbability(ArmInstr* armInstr, double takenProbability) {
    branch_probability_.put(armInstr, takenProbability);
}

/// Retrieves the probability that the arm is taken
///
double ProfiledInformation::getBranchProbability(ArmInstr* armInstr) {
    assert (branch_probability_.containsKey(armInstr));
    return branch_probability_.get(armInstr);
}

/// Returns true if firstArm is taken more than the second arm.
bool ProfiledInformation::isHotterArm(ArmInstr* thinkHotArm, ArmInstr* thinkSlowArm) {
    return getBranchProbability(thinkHotArm) > getBranchProbability(thinkSlowArm);
}

}
#endif // VMCFG_HALFMOON
