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

T08: check
	@echo "-------- Example: Lua --------"
	cd lua-5.2.0 && make FLASCC="$(call unixpath,$(FLASCC))" NP_FLASCC="$(call nativepath,$(FLASCC))" OPT_FLAGS="$(OPT_CFLAGS) $(EXTRACFLAGS)" flash

	@echo && echo "Compiling test app using SWC:"
	"$(FLEX)/bin/mxmlc" -library-path+=lua.swc luatest.mxml -debug=$(MXMLC_DEBUG) -o luatest.swf
	make starlingdemo

T08Debug:
	make T08 OPT_CFLAGS="-O0 -g" MXMLC_DEBUG=true

starlingdemo:
	cd LuaStarling && "$(FLEX)/bin/mxmlc" -library-path+=../lua.swc LuaStarling.as -debug=$(MXMLC_DEBUG) -o LuaStarling.swf

include ../Makefile.common

clean:
	cd lua-5.2.0 && make clean
	rm -rf build install
	rm -f luatest.swf lua.swc LuaStarling/LuaStarling.swf
