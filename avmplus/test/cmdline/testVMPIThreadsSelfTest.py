#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    r.run_test('VMPIThreads(-Dselftest=vmpi,threads)','%s -Dselftest=vmpi,threads' % r.avm,expectedcode=0,
               expectedout=[
                            "\['start', 'vmpi', 'threads'\]",
                            "\['test', 'vmpi', 'threads', 'mutexes'\]",
                            "\['pass', 'vmpi', 'threads', 'mutexes'\]",
                            "\['test', 'vmpi', 'threads', 'conditions'\]",
                            "\['pass', 'vmpi', 'threads', 'conditions'\]",
                            "\['test', 'vmpi', 'threads', 'atomic_increment_without_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'atomic_increment_without_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'atomic_decrement_without_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'atomic_decrement_without_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'compare_and_swap_without_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'compare_and_swap_without_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'atomic_increment_with_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'atomic_increment_with_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'atomic_decrement_with_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'atomic_decrement_with_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'compare_and_swap_with_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'compare_and_swap_with_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'memory_barrier'\]",
                            "\['pass', 'vmpi', 'threads', 'memory_barrier'\]",
                            "\['test', 'vmpi', 'threads', 'try_lock'\]",
                            "\['pass', 'vmpi', 'threads', 'try_lock'\]",
                            "\['test', 'vmpi', 'threads', 'condition_with_wait'\]",
                            "\['pass', 'vmpi', 'threads', 'condition_with_wait'\]",
                            "\['test', 'vmpi', 'threads', 'sleep'\]",
                            "\['pass', 'vmpi', 'threads', 'sleep'\]",
                            "\['test', 'vmpi', 'threads', 'detach'\]",
                            "\['pass', 'vmpi', 'threads', 'detach'\]",
                            "\['test', 'vmpi', 'threads', 'thread_attributes'\]",
                            "\['pass', 'vmpi', 'threads', 'thread_attributes'\]",
                            "\['end', 'vmpi', 'threads'\]"
                            ]
   )

if __name__ == '__main__':
    r=RunTestLib()
    run()
