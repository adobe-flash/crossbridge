/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

raytrace workers demo:

to compile:
$ java -jar asc.jar -import playerglobal.abc -import builtin.abc -swf RayTracer,640,480,100 -AS3 raytradeWorkers.as
raytraceWorkers.swf, 6394 bytes written

to change the number of workers edit the source: numWorkers=1; to the desired number of workers

note: if the 320/(numworker+1) does not divide evenly the program changes numWorkers to the next lowest number dividing
equally. So for example if you set numWorkers=2 , numWorkers gets set to 1 since 320/3 does not divide evenly.


measuring overhead workers:
Overhead.as is a test to measure the memory footprint of increasing workers

to change the number of workers:
edit num.as, change nworkers=8; to the desired number of workers

to build:
$ java -jar asc.jar -import playerglobal.abc -import builtin.abc -swf Overhead,640,480,100 -AS3 Overhead.as
Overhead.swf, 2874 bytes written

to run:
run the player
I use Activity Monitor on OS/X to view the memory footprint
when viewing the memory watch carefully as the memory will spike and after 10s the worker will stop() and much of the memory will be reclaimed.

 
