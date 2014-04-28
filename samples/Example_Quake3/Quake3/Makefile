$?BASEQ3DIR=/xxx
$?CC=$(ALCHEMY)/usr/bin/gcc
$?LLC=$(ALCHEMY)/usr/bin/llc
$?LINK=$(ALCHEMY)/usr/bin/llvm-link
$?OPT=$(ALCHEMY)/usr/bin/opt
$?EXTRACFLAGS=

$?BASECFLAGS= -Dstricmp=strcasecmp -DNO_VM_COMPILED -DHARD_LINKED -DARCH_STRING='"AVM2"' -DAPIENTRY="" -DSTANDALONE -DBOTLIB $(EXTRACFLAGS) \
-DOPENGL -DCGAME -DGAME_HARD_LINKED -DC_ONLY -Wno-pointer-sign -I$(ALCHEMY)/usr/include/SDL -I$(GLS3D)/install/usr/include $(EXTRACFLAGS) \
-Icode \
-Icode/q3_ui \
-Icode/qcommon \
-Icode/sys \
-Icode/renderer \
-Icode/ui \

$?CFLAGS=-O4
$?BUILDDIR=build

OBJS = code/qcommon/cm_load.o \
code/qcommon/cm_patch.o \
code/qcommon/cm_polylib.o \
code/qcommon/cm_test.o \
code/qcommon/cm_trace.o \
code/qcommon/cmd.o \
code/qcommon/common.o \
code/qcommon/cvar.o \
code/qcommon/files.o \
code/qcommon/huffman.o \
code/qcommon/ioapi.o \
code/qcommon/md4.o \
code/qcommon/md5.o \
code/qcommon/msg.o \
code/qcommon/net_chan.o \
code/qcommon/net_ip.o \
code/qcommon/puff.o \
code/qcommon/q_math.o \
code/qcommon/q_shared.o \
code/qcommon/unzip.o \
code/qcommon/vm.o \
code/qcommon/vm_interpreted.o \
code/sys/con_tty.o \
code/sys/sys_main.o \
code/sys/sys_unix.o \
code/sys/con_log.o \
code/renderer/tr_animation.o \
code/renderer/tr_backend.o \
code/renderer/tr_bsp.o \
code/renderer/tr_cmds.o \
code/renderer/tr_curve.o \
code/renderer/tr_flares.o \
code/renderer/tr_font.o \
code/renderer/tr_image.o \
code/renderer/tr_image_bmp.o \
code/renderer/tr_image_jpg.o \
code/renderer/tr_image_pcx.o \
code/renderer/tr_image_png.o \
code/renderer/tr_image_tga.o \
code/renderer/tr_init.o \
code/renderer/tr_light.o \
code/renderer/tr_main.o \
code/renderer/tr_marks.o \
code/renderer/tr_mesh.o \
code/renderer/tr_model.o \
code/renderer/tr_model_iqm.o \
code/renderer/tr_noise.o \
code/renderer/tr_scene.o \
code/renderer/tr_shade.o \
code/renderer/tr_shade_calc.o \
code/renderer/tr_shader.o \
code/renderer/tr_shadows.o \
code/renderer/tr_sky.o \
code/renderer/tr_surface.o \
code/renderer/tr_world.o \
code/qcommon/vm_none.o \
code/sdl/sdl_gamma.o \
code/sdl/sdl_glimp.o \
code/sdl/sdl_input.o \
code/sdl/sdl_snd.o \
code/zlib/adler32.o \
code/zlib/crc32.o \
code/zlib/inffast.o \
code/zlib/inflate.o \
code/zlib/inftrees.o \
code/zlib/zutil.o \
code/q3_ui/ui_addbots.o \
code/q3_ui/ui_atoms.o \
code/q3_ui/ui_cdkey.o \
code/q3_ui/ui_cinematics.o \
code/q3_ui/ui_confirm.o \
code/q3_ui/ui_connect.o \
code/q3_ui/ui_controls2.o \
code/q3_ui/ui_credits.o \
code/q3_ui/ui_demo2.o \
code/q3_ui/ui_display.o \
code/q3_ui/ui_gameinfo.o \
code/q3_ui/ui_ingame.o \
code/q3_ui/ui_loadconfig.o \
code/q3_ui/ui_login.o \
code/q3_ui/ui_main.o \
code/q3_ui/ui_menu.o \
code/q3_ui/ui_mfield.o \
code/q3_ui/ui_mods.o \
code/q3_ui/ui_network.o \
code/q3_ui/ui_options.o \
code/q3_ui/ui_playermodel.o \
code/q3_ui/ui_players.o \
code/q3_ui/ui_playersettings.o \
code/q3_ui/ui_preferences.o \
code/q3_ui/ui_qmenu.o \
code/q3_ui/ui_removebots.o \
code/q3_ui/ui_saveconfig.o \
code/q3_ui/ui_serverinfo.o \
code/q3_ui/ui_servers2.o \
code/q3_ui/ui_setup.o \
code/q3_ui/ui_sound.o \
code/q3_ui/ui_sparena.o \
code/q3_ui/ui_specifyleague.o \
code/q3_ui/ui_specifyserver.o \
code/q3_ui/ui_splevel.o \
code/q3_ui/ui_sppostgame.o \
code/q3_ui/ui_spreset.o \
code/q3_ui/ui_spskill.o \
code/q3_ui/ui_startserver.o \
code/q3_ui/ui_team.o \
code/q3_ui/ui_teamorders.o \
code/q3_ui/ui_video.o \
code/server/sv_bot.o \
code/server/sv_ccmds.o \
code/server/sv_client.o \
code/server/sv_game.o \
code/server/sv_init.o \
code/server/sv_main.o \
code/server/sv_net_chan.o \
code/server/sv_snapshot.o \
code/server/sv_world.o \
code/client/cl_avi.o \
code/client/cl_cgame.o \
code/client/cl_cin.o \
code/client/cl_console.o \
code/client/cl_curl.o \
code/client/cl_input.o \
code/client/cl_keys.o \
code/client/cl_main.o \
code/client/cl_net_chan.o \
code/client/cl_parse.o \
code/client/cl_scrn.o \
code/client/cl_ui.o \
code/client/libmumblelink.o \
code/client/qal.o \
code/client/snd_adpcm.o \
code/client/snd_codec.o \
code/client/snd_codec_ogg.o \
code/client/snd_codec_wav.o \
code/client/snd_dma.o \
code/client/snd_main.o \
code/client/snd_mem.o \
code/client/snd_mix.o \
code/client/snd_openal.o \
code/client/snd_wavelet.o \
code/botlib/be_aas_bspq3.o \
code/botlib/be_aas_cluster.o \
code/botlib/be_aas_debug.o \
code/botlib/be_aas_entity.o \
code/botlib/be_aas_file.o \
code/botlib/be_aas_main.o \
code/botlib/be_aas_move.o \
code/botlib/be_aas_optimize.o \
code/botlib/be_aas_reach.o \
code/botlib/be_aas_route.o \
code/botlib/be_aas_routealt.o \
code/botlib/be_aas_sample.o \
code/botlib/be_ai_char.o \
code/botlib/be_ai_chat.o \
code/botlib/be_ai_gen.o \
code/botlib/be_ai_goal.o \
code/botlib/be_ai_move.o \
code/botlib/be_ai_weap.o \
code/botlib/be_ai_weight.o \
code/botlib/be_ea.o \
code/botlib/be_interface.o \
code/botlib/l_crc.o \
code/botlib/l_libvar.o \
code/botlib/l_log.o \
code/botlib/l_memory.o \
code/botlib/l_precomp.o \
code/botlib/l_script.o \
code/botlib/l_struct.o \
code/jpeg-8c/jaricom.o \
code/jpeg-8c/jcapimin.o \
code/jpeg-8c/jcapistd.o \
code/jpeg-8c/jcarith.o \
code/jpeg-8c/jccoefct.o \
code/jpeg-8c/jccolor.o \
code/jpeg-8c/jcdctmgr.o \
code/jpeg-8c/jchuff.o \
code/jpeg-8c/jcinit.o \
code/jpeg-8c/jcmainct.o \
code/jpeg-8c/jcmarker.o \
code/jpeg-8c/jcmaster.o \
code/jpeg-8c/jcomapi.o \
code/jpeg-8c/jcparam.o \
code/jpeg-8c/jcprepct.o \
code/jpeg-8c/jcsample.o \
code/jpeg-8c/jctrans.o \
code/jpeg-8c/jdapimin.o \
code/jpeg-8c/jdapistd.o \
code/jpeg-8c/jdarith.o \
code/jpeg-8c/jdatadst.o \
code/jpeg-8c/jdatasrc.o \
code/jpeg-8c/jdcoefct.o \
code/jpeg-8c/jdcolor.o \
code/jpeg-8c/jddctmgr.o \
code/jpeg-8c/jdhuff.o \
code/jpeg-8c/jdinput.o \
code/jpeg-8c/jdmainct.o \
code/jpeg-8c/jdmarker.o \
code/jpeg-8c/jdmaster.o \
code/jpeg-8c/jdmerge.o \
code/jpeg-8c/jdpostct.o \
code/jpeg-8c/jdsample.o \
code/jpeg-8c/jdtrans.o \
code/jpeg-8c/jerror.o \
code/jpeg-8c/jfdctflt.o \
code/jpeg-8c/jfdctfst.o \
code/jpeg-8c/jfdctint.o \
code/jpeg-8c/jidctflt.o \
code/jpeg-8c/jidctfst.o \
code/jpeg-8c/jidctint.o \
code/jpeg-8c/jmemmgr.o \
code/jpeg-8c/jmemnobs.o \
code/jpeg-8c/jquant1.o \
code/jpeg-8c/jquant2.o \
code/jpeg-8c/jutils.o \
code/ui/ui_syscalls.o


CRAP = code/cgame/cg_consolecmds.o \
code/cgame/cg_draw.o \
code/cgame/cg_drawtools.o \
code/cgame/cg_effects.o \
code/cgame/cg_ents.o \
code/cgame/cg_event.o \
code/cgame/cg_info.o \
code/cgame/cg_localents.o \
code/cgame/cg_main.o \
code/cgame/cg_marks.o \
code/cgame/cg_newdraw.o \
code/cgame/cg_particles.o \
code/cgame/cg_players.o \
code/cgame/cg_playerstate.o \
code/cgame/cg_predict.o \
code/cgame/cg_scoreboard.o \
code/cgame/cg_servercmds.o \
code/cgame/cg_snapshot.o \
code/cgame/cg_syscalls.o \
code/cgame/cg_view.o \
code/cgame/cg_weapons.o

all:
	rm -rf $(BUILDDIR)
	mkdir -p $(BUILDDIR)

	rm -rf q3vfs
	mkdir -p q3vfs

	make q3

.c.o:
	$(CC) $(CFLAGS) -emit-llvm $(BASECFLAGS) -c $< -o $(BUILDDIR)/$(@F)

q3: $(OBJS)
	mkdir -p $(BUILDDIR)/q3vfs/baseq3
	mkdir -p $(BUILDDIR)/q3vfs/home/baseq3
	cp -fr $(BASEQ3DIR)/* $(BUILDDIR)/q3vfs/baseq3/

	$(ALCHEMY)/usr/bin/genfs --type=http $(BUILDDIR)/q3vfs/ "q3vfs/baseq3"
	mv q3vfs/baseq3_manifest.as $(BUILDDIR)/manifest.as
	cp -f $(ALCHEMY)/usr/share/HTTPBackingStore.as $(BUILDDIR)/HTTPBackingStore.as
	cp -f Console.as $(BUILDDIR)/
	cd $(BUILDDIR) && $(LINK) -o quake3.bc *.o
	cd $(BUILDDIR) && $(LLC) -jvmopt=-Xmx2G quake3.bc -filetype=obj -o quake3.abc
	make q3link

q3link:
	cd $(BUILDDIR) && java -classpath $(ALCHEMY)/usr/lib/asc.jar macromedia.asc.embedding.ScriptCompiler \
		-abcfuture -AS3 -swf flascc.preloader.DefaultPreLoader,1024,768 \
		-import $(ALCHEMY)/usr/lib/builtin.abc \
		-import $(ALCHEMY)/usr/lib/playerglobal.abc \
		$(ALCHEMY)/usr/share/IBackingStore.as \
		$(ALCHEMY)/usr/share/InMemoryBackingStore.as \
		$(ALCHEMY)/usr/share/DefaultPreLoader.as \
		HttpBackingStore.as -outdir . -out preloader
	cd $(BUILDDIR) && java -classpath $(ALCHEMY)/usr/lib/asc.jar macromedia.asc.embedding.ScriptCompiler \
		-abcfuture -AS3 -strict -optimize \
		-import $(ALCHEMY)/usr/lib/builtin.abc \
		-import $(ALCHEMY)/usr/lib/playerglobal.abc \
		-import $(GLS3D)/install/usr/lib/libGL.abc \
		-import $(ALCHEMY)/usr/lib/ISpecialFile.abc \
		-import $(ALCHEMY)/usr/lib/IBackingStore.abc \
		-import $(ALCHEMY)/usr/lib/IBackingStoreManager.abc \
		-import $(ALCHEMY)/usr/lib/CModule.abc \
		-import $(ALCHEMY)/usr/lib/C_Run.abc \
		-import $(ALCHEMY)/usr/lib/BinaryData.abc \
		-import $(ALCHEMY)/usr/lib/PlayerPosix.abc \
		Console.as -outdir . -out Console
	cd $(BUILDDIR) && $(CC) $(CFLAGS) quake3.abc $(GLS3D)/install/usr/lib/libGL.abc $(GLS3D)/install/usr/lib/libGL.a -lSDL -lvgl \
		-Wl,--warn-unresolved-symbols -symbol-abc=Console.abc -swf-version=16 -swf-preloader=preloader.swf -emit-swf -swf-size=1024x768 -o ../Quake3.swf

clean:
	rm -rf $(BUILDDIR) *.swf
