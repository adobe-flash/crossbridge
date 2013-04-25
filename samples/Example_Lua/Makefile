$?EXTRACFLAGS=
$?OPT_CFLAGS=-O4
$?MXMLC_DEBUG=false

T08: check
	@echo "-------- Example: Lua --------"
	cd lua-5.2.0 && make FLASCC="$(call unixpath,$(FLASCC))" NP_FLASCC="$(call nativepath,$(FLASCC))" OPT_FLAGS="$(OPT_CFLAGS) $(EXTRACFLAGS)" flash

	@echo && echo "Compiling test app using SWC:"
	"$(FLEX)/bin/mxmlc" -compiler.omit-trace-statements=false -library-path+=lua.swc luatest.mxml -debug=$(MXMLC_DEBUG) -static-link-runtime-shared-libraries=true -o luatest.swf
	make starlingdemo

T08Debug:
	make T08 OPT_CFLAGS="-O0 -g" MXMLC_DEBUG=true

starlingdemo:
	cd LuaStarling && "$(FLEX)/bin/mxmlc" -static-link-runtime-shared-libraries -library-path+=../lua.swc LuaStarling.as -debug=$(MXMLC_DEBUG) -o LuaStarling.swf

include ../Makefile.common

clean:
	cd lua-5.2.0 && make clean
	rm -rf build install
	rm -f luatest.swf lua.swc LuaStarling/LuaStarling.swf
