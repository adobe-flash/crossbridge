# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

Instructions for running performance tests

- sunspider is a collection of tests from Apple http://webkit.org/perf/sunspider-0.9/sunspider.html, the tests are from computer
    shootout, and other sources
- jsbench is the port of javagrande2 tests to javascript, work done by UC Irvine, these tests take about 50-60 minutes, not run by default

the tests are meant to be run against tamarin-central and/or tamarin-tracing

usage example:
$ export AVM=c:/dev/tamarin-central/platform/win32/obj_8/shell/Release/avmplus.exe
$ export AVM2=c:/dev/tamarin-tracing/platform/win32/obj_8/shell/Release/avmplus.exe
$ export ASC=c:/dev/asc/asc.jar
$ export BUILTINABC=c:/dev/tamarin-central/core/builtin.abc
$ ./runtests.py


to specify the tests to run
$ ./runtests.py sunspider/
or 
$ ./runtests.py sunspider/s3d-cube.as

to skip tests edit testconfig.txt

testconfig.txt contains configuration settings:

example output:
$ ./runtests.py --config=jsbench
$ ./runtests.py --config=all