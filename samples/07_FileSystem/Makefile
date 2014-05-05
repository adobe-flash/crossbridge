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

T07: check
	@echo "-------- Sample 7 --------"
	"$(FLASCC)/usr/bin/genfs" vfs testvfs
	cp "$(FLASCC)/usr/share/HTTPBackingStore.as" .
	cp "$(FLASCC)/usr/share/LSOBackingStore.as" .
	"$(FLASCC)/usr/bin/genfs" --type=http webvfs testwebvfs
	mv testwebvfs_manifest.as manifest.as
	$(AS3COMPILERARGS)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				HTTPBackingStore.as LSOBackingStore.as testvfsRootFSBackingStore.as -outdir . -out VFS
	$(AS3COMPILERARGS)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import VFS.abc \
				Console.as -outdir . -out Console
	"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -O3 vfstest.cpp VFS.abc $(FLASCC)/usr/lib/AlcVFSZip.abc -symbol-abc=Console.abc -emit-swf -swf-version=$(SWF_VERSION) -swf-size=$(SWF_SIZE) -o vfstest.swf

include ../Makefile.common

clean:
	rm -f *.swf *.bc *.abc testvfs* HTTPBackingStore.as LSOBackingStore.as testwebvfs*
