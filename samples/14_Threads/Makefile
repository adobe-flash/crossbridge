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

SWFS=atomic.swf flash++flavors.swf flash++ui.swf openmp.swf pthreads.swf sema.swf workers.swf

T14: check $(SWFS)

atomic.swf: atomic.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread atomic.cpp -o atomic.swf

pthreads.swf: pthreads.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread pthreads.cpp -o pthreads.swf

sema.swf: sema.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread sema.cpp -o sema.swf

workers.swf: workers.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread workers.cpp -o workers.swf

openmp.swf: openmp.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread -fopenmp openmp.cpp -o openmp.swf

flash++flavors.swf: flash++flavors.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread flash++flavors.cpp -lFlash++ -lAS3++ -o flash++flavors.swf

flash++ui.swf: flash++ui.cpp
	$(FLASCC)/usr/bin/g++ -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -O4 -flto-api=exports.txt -pthread flash++ui.cpp -lFlash++ -lAS3++ -o flash++ui.swf

include ../Makefile.common

clean:
	rm -f $(SWFS)
