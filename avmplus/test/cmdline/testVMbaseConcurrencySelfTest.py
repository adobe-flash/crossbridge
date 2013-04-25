#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    r.run_test('VMbaseConcurrency(-Dselftest=vmbase,concurrency)','%s -Dselftest=vmbase,concurrency' % r.avm,expectedcode=0,
               expectedout=[
                            "\['start', 'vmbase', 'concurrency'\]",
                            "\['test', 'vmbase', 'concurrency', 'mutexes'\]",
                            "\['pass', 'vmbase', 'concurrency', 'mutexes'\]",
                            "\['test', 'vmbase', 'concurrency', 'conditions'\]",
                            "\['pass', 'vmbase', 'concurrency', 'conditions'\]",
                            "\['test', 'vmbase', 'concurrency', 'atomic_counter'\]",
                            "\['pass', 'vmbase', 'concurrency', 'atomic_counter'\]",
                            "\['test', 'vmbase', 'concurrency', 'compare_and_swap_without_barrier'\]",
                            "\['pass', 'vmbase', 'concurrency', 'compare_and_swap_without_barrier'\]",
                            "\['test', 'vmbase', 'concurrency', 'compare_and_swap_with_barrier'\]",
                            "\['pass', 'vmbase', 'concurrency', 'compare_and_swap_with_barrier'\]",
                            "\['test', 'vmbase', 'concurrency', 'memory_barrier'\]",
                            "\['pass', 'vmbase', 'concurrency', 'memory_barrier'\]",
                            "\['test', 'vmbase', 'concurrency', 'condition_with_wait'\]",
                            "\['pass', 'vmbase', 'concurrency', 'condition_with_wait'\]",
                            "\['test', 'vmbase', 'concurrency', 'sleep'\]",
                            "\['pass', 'vmbase', 'concurrency', 'sleep'\]",
                            "\['test', 'vmbase', 'concurrency', 'vmthreadlocal'\]",
                            "\['pass', 'vmbase', 'concurrency', 'vmthreadlocal'\]",
                            "\['test', 'vmbase', 'concurrency', 'join'\]",
                            "\['pass', 'vmbase', 'concurrency', 'join'\]",
                            "\['end', 'vmbase', 'concurrency'\]"
                            ]
   )

if __name__ == '__main__':
    r=RunTestLib()
    run()
