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

.PHONY: SpaceTriangle

T12: check

	@echo "-------- Sample 12 --------"

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		AGALAssembler/*.as AGALAssembler/macro/*.as -outdir . -out AGAL

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		Console.as -outdir . -out Console

	java -jar $(call nativepath, $(FLASCC)/usr/lib/as3wig.jar) -i AGAL.abc -o AGAL
	"$(FLASCC)/usr/bin/g++" AGAL.cpp -c -emit-llvm &> AGAL.txt

	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -fllvm-llc-opt=-ascopt=-import -fllvm-llc-opt=-ascopt=$(call nativepath,AGAL.abc) -I. AGAL.o stage3d.cpp AGAL.abc -lAS3++ -lFlash++ \
		-O4 -flto-api=exports.txt -symbol-abc=Console.abc -swf-size=320x480 -emit-swf -o stage3d.swf

	make SpaceTriangle

SpaceTriangle: check

	$(AS3COMPILERARGS) -AS3 -strict \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/CModule.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		SpaceTriangle/Console.as -outdir SpaceTriangle -out Console

	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) \
		-fllvm-llc-opt=-ascopt=-import \
		-fllvm-llc-opt=-ascopt=$(call nativepath,AGAL.abc) \
		-I. -ISpaceTriangle SpaceTriangle/*.cpp \
		AGAL.o AGAL.abc -lAS3++ -lFlash++ \
		-O4 -flto-api=exports.txt -symbol-abc=SpaceTriangle/Console.abc -swf-size=320x480 -emit-swf -o SpaceTriangle.swf

clean:
	rm -f stage3d.swf SpaceTriangle.swf rm AGAL.* AGAL_* *.abc *.o SpaceTriangle/*.abc

include ../Makefile.common
