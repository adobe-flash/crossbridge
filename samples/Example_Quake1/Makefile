$?PAK0FILE=baseq1/pak0.pak
$?BUILDDIR=build
$?QUAKECFLAGS=$(BASE_CFLAGS) -D_THREAD_SAFE -DSDL -Wno-pointer-sign `"$(call unixpath,$(FLASCC)/usr/bin/sdl-config)" --cflags` $(EXTRACFLAGS)
$?DEBUG=FALSE
$?EXTRACFLAGS= 
JVMARGS:=-Xmx1G

ifeq ("$(DEBUG)","FALSE")
	$?CFLAGS=-O4 -flto-api=../exports.txt -fno-stack-protector $(EXTRACFLAGS)
else
	$?CFLAGS=-O0 -g $(EXTRACFLAGS)
endif

OBJS = sdlquake-1.0.9/cd_null.o sdlquake-1.0.9/chase.o sdlquake-1.0.9/cl_demo.o sdlquake-1.0.9/cl_input.o sdlquake-1.0.9/cl_main.o sdlquake-1.0.9/cl_parse.o sdlquake-1.0.9/cl_tent.o sdlquake-1.0.9/cmd.o sdlquake-1.0.9/common.o sdlquake-1.0.9/console.o sdlquake-1.0.9/crc.o sdlquake-1.0.9/cvar.o sdlquake-1.0.9/d_edge.o sdlquake-1.0.9/d_fill.o sdlquake-1.0.9/d_init.o sdlquake-1.0.9/d_modech.o sdlquake-1.0.9/d_part.o sdlquake-1.0.9/d_polyse.o sdlquake-1.0.9/d_scan.o sdlquake-1.0.9/d_sky.o sdlquake-1.0.9/d_sprite.o sdlquake-1.0.9/d_surf.o sdlquake-1.0.9/d_zpoint.o sdlquake-1.0.9/draw.o sdlquake-1.0.9/host.o sdlquake-1.0.9/host_cmd.o sdlquake-1.0.9/keys.o sdlquake-1.0.9/mathlib.o sdlquake-1.0.9/menu.o sdlquake-1.0.9/model.o sdlquake-1.0.9/net_main.o sdlquake-1.0.9/net_none.o sdlquake-1.0.9/net_vcr.o sdlquake-1.0.9/net_loop.o sdlquake-1.0.9/pr_cmds.o sdlquake-1.0.9/pr_edict.o sdlquake-1.0.9/pr_exec.o sdlquake-1.0.9/r_aclip.o sdlquake-1.0.9/r_alias.o sdlquake-1.0.9/r_bsp.o sdlquake-1.0.9/r_draw.o sdlquake-1.0.9/r_edge.o sdlquake-1.0.9/r_efrag.o sdlquake-1.0.9/r_light.o sdlquake-1.0.9/r_main.o sdlquake-1.0.9/r_misc.o sdlquake-1.0.9/r_part.o sdlquake-1.0.9/r_sky.o sdlquake-1.0.9/r_sprite.o sdlquake-1.0.9/r_surf.o sdlquake-1.0.9/r_vars.o sdlquake-1.0.9/sbar.o sdlquake-1.0.9/screen.o sdlquake-1.0.9/snd_dma.o sdlquake-1.0.9/snd_mem.o sdlquake-1.0.9/snd_mix.o sdlquake-1.0.9/snd_sdl.o sdlquake-1.0.9/sv_main.o sdlquake-1.0.9/sv_move.o sdlquake-1.0.9/sv_phys.o sdlquake-1.0.9/sv_user.o sdlquake-1.0.9/sys_sdl.o sdlquake-1.0.9/vid_sdl.o sdlquake-1.0.9/view.o sdlquake-1.0.9/wad.o sdlquake-1.0.9/world.o sdlquake-1.0.9/zone.o sdlquake-1.0.9/d_vars.o sdlquake-1.0.9/nonintel.o

q1: setup
	make q1build

q1build: $(OBJS)
	@echo "-------- Example: Quake 1 --------"
	cp -f $(PAK0FILE) $(BUILDDIR)/q1vfs/id1/pak0.pak
	chmod 777 $(BUILDDIR)/q1vfs/id1/pak0.pak
	cd $(BUILDDIR) && "$(FLASCC)/usr/bin/genfs" q1vfs q1vfs
	
	cp -f sdlquake-1.0.9/Console.as $(BUILDDIR)/

	cd $(BUILDDIR) && $(AS3COMPILERARGS) \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
		Console.as 

	cd $(BUILDDIR) && $(AS3COMPILERARGS) \
		-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
		-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
		q1vfs*.as
	make -j2 link linkmt

link:
	cd $(BUILDDIR) && "$(FLASCC)/usr/bin/gcc" $(CFLAGS) $(OBJS) -lSDL -lvgl -o ../Quake1.swf   q1vfs*.abc $(FLASCC)/usr/lib/AlcVFSZip.abc -emit-swf -symbol-abc=Console.abc -swf-size=1024x768 -swf-version=17

linkmt:
	cd $(BUILDDIR) && "$(FLASCC)/usr/bin/gcc" $(CFLAGS) $(OBJS) -lSDL -lvgl -o ../Quake1MT.swf q1vfs*.abc $(FLASCC)/usr/lib/AlcVFSZip.abc -emit-swf -symbol-abc=Console.abc -swf-size=1024x768 -pthread

include ../Makefile.common

setup:	check
	@if [ -e $(PAK0FILE) ] ; then true ; \
	else echo "Couldn't locate Quake1 PAK file, please invoke make with \"make PAK0FILE=/path/to/your/quake1/pak0.pak ...\"" ; exit 1 ; \
	fi

	mkdir -p $(BUILDDIR)/q1vfs/id1
	mkdir -p $(BUILDDIR)/sdlquake-1.0.9	
	
clean:
	rm -rf $(BUILDDIR) *.swf

.c.o:
	"$(FLASCC)/usr/bin/gcc" $(CFLAGS) $(QUAKECFLAGS) -c $< -o $(BUILDDIR)/sdlquake-1.0.9/$(@F)
