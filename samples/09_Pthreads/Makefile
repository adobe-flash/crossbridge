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

T09: check T09_1 T09_2 T09_3 T09_4 T09_5
    
T09_1:
	@echo "-------- Sample 9 / 1 --------"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) pthreads.cpp -lAS3++ -lFlash++ -pthread -O4 -flto-api=../exports.txt -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o pthreads.swf

T09_2:
	@echo "-------- Sample 9 / 2 --------"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -O4 -flto-api=../exports.txt sound.cpp -lAS3++ -lFlash++ -pthread -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o sound.swf

T09_3:
	@echo "-------- Sample 9 / 3 --------"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -pthread pthread_swc.cpp -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o pthread_swc1.swf

T09_4:
	@echo "-------- Sample 9 / 4 --------"
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -pthread pthread_swc.cpp -emit-swc=sample.pthread -o pthread.swc

T09_5:
	@echo "-------- Sample 9 / 5 --------"
	"$(FLEX)/bin/mxmlc" -load-config+=./../.flexConfig.xml -default-script-limits 1000000 5 -library-path=pthread.swc -debug=$(MXMLC_DEBUG) pthread_swc.as -o pthread_swc2.swf

include ../Makefile.common

clean:
	rm -f *.swf *.swc
    
debug:
	"$(FLASCC)/usr/bin/gdb" pthread_swc2.swf
