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

T01: check
	@echo "-------- Example rand_points --------"
	@echo "First let's compile it as a projector:"
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) -O3 -lSDL -lpthread rand_points.cpp -o rand_points.exe
	@echo "Now lets compile it as a SWF:"
	"$(FLASCC)/usr/bin/gcc" $(BASE_CFLAGS) -O3 -lSDL -lpthread rand_points.cpp -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o rand_points.swf

include ../Makefile.common

clean:
	@rm -f *.swf *.swc *.bc *.abc *.exe
