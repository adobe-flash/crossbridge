#
# =BEGIN MIT LICENSE
# 
# The MIT License (MIT)
#
# Copyright (c) 2014 The CrossBridge Team
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# =END MIT LICENSE
#

BUILD=$(PWD)/build
INSTALL=$(PWD)/install
SRCROOT=$(PWD)/

swc: check
	cd swc && $(ENV) cmake -D SDK="$(FLASCC)" .
	cd swc && $(ENV) make -j6
	mv swc/glsl2agal.swc bin/
	mv swc/glsl2agalopt bin/

win:
	python $(FLASCC)/usr/bin/projector-dis.py bin/glsl2agalopt
	$(FLASCC)/usr/bin/avmshell external/projectormake.abc -- -o bin/glsl2agalopt.exe external/avmshell.exe output.swf -- -osr=1
	rm -f output*

example: check
	cd examples/basic && $(FLEX)/bin/mxmlc \
		-static-link-runtime-shared-libraries \
		-omit-trace-statements=false \
		-library-path+=../../bin/glsl2agal.swc \
		GLSLCompiler.mxml \
		-o GLSLCompiler.swf

check:
	@if [ -d $(FLASCC)/usr/bin ] ; then true ; \
	else echo "Couldn't locate FLASCC sdk directory, please invoke make with \"make FLASCC=/path/to/FLASCC/sdk ...\"" ; exit 1 ; \
	fi

	@if [ -d "$(FLEX)/bin" ] ; then true ; \
	else echo "Couldn't locate Flex sdk directory, please invoke make with \"make FLEX=/path/to/flex  ...\"" ; exit 1 ; \
	fi

include ../Makefile.common

clean:
	rm -rf swc/cmake_install.cmake swc/CMakeCache.txt swc/CMakeFiles swc/Makefile
