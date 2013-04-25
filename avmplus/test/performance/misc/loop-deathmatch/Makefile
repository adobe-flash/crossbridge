# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

.SUFFIXES: .as .abs .abc

.as.abc:
	asc -strict -AS3 $<

.abs.abc:
	abcasm $<

AVM=avmshell -Dnodebugger
TARGETS=bytearray-loop-forward.abc \
	bytearray-loop-backward.abc \
	bytearray-unrolled-forward.abc \
	bytearray-unrolled-backward.abc \
	mops-loop-forward.abc \
	mops-unrolled-forward.abc \
	vector-loop-forward.abc \
	vector-unrolled-forward.abc

test: $(TARGETS)
	for i in $(TARGETS); do \
		$(AVM) $$i ; \
	done

clean:
	rm -f *.abc *~
