# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

all: log2s-32 log2s-64 dinkuuid jit64.so jit32.so

log2s-32: log2s.cpp
	g++ -m32 log2s.cpp -o log2s-32

log2s-64: log2s.cpp
	g++ -m64 log2s.cpp -o log2s-64

dinkuuid: dinkuuid.cpp
	g++ dinkuuid.cpp -o dinkuuid

jit64.so: jit.s dinkuuid
	gcc -m64 -shared -nostartfiles -nodefaultlibs jit.s -o jit64.so
	./dinkuuid jit64.so

jit32.so: jit.s dinkuuid
	gcc -m32 -shared -nostartfiles -nodefaultlibs jit.s -o jit32.so
	./dinkuuid jit32.so

clean:
	rm -f dinkuuid log2s-* jit*.so

install:
	cp ./jit*.so /tmp
