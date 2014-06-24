# This file will delete your entire #SharedObjects directory, then build and launch
# setup.swf.  Setup.swf populates the #SharedObjects directory with a bunch of files
# which will be used for testing.

AS3COMP=java -jar $(call nativepath,$(FLASCC)/usr/lib/asc2.jar) -abcfuture -AS3 -merge -md

$?SRCROOT=$(PWD)/../../..
$?UNAME=$(shell uname -s)

# Be sure to launch the Mac player with "Flash Player.app" intead of going into that
# directory for the binary.  Otherwise you'll hit security problems because this
# swf exits itself.
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?SHARED_OBJECTS_DIR="$(USERPROFILE)/AppData/Roaming/Macromedia/Flash Player/\#SharedObjects"
	$?PLAYER=$(FLASH_PLAYER_EXE)
else
	$?nativepath=$(abspath $(1))
	$?SHARED_OBJECTS_DIR="$(HOME)/Library/Preferences/Macromedia/Flash Player/\#SharedObjects/"
	$?PLAYER=open -a "$(FLASH_PLAYER_EXE)"
endif

# Note that we move the framework directory so it has the same name/location as the test swf.  This allows
# LSOs to be shared between SWFs on Windows.
DoSetup:

	rm -rf ../tests/filestores/embedded/..dotdot
	@/bin/echo -n abcde > ../tests/filestores/embedded/..dotdot

	rm -rf ../tests/filestores/embedded/.dot
	@/bin/echo -n abcde > ../tests/filestores/embedded/.dot

	rm -rf ../tests/filestores/embedded/..dotdotdir
	mkdir ../tests/filestores/embedded/..dotdotdir

	rm -rf ../tests/filestores/embedded/.dotdir
	mkdir ../tests/filestores/embedded/.dotdir

	rm -f ../tests/filestores/embedded/..dotdotdir/file.txt
	@/bin/echo -n abcde > ../tests/filestores/embedded/..dotdotdir/file.txt

	rm -f ../tests/filestores/embedded/.dotdir/file.txt
	@/bin/echo -n abcde > ../tests/filestores/embedded/.dotdir/file.txt

	rm -rf ../tests/filestores/compressed/compressed/..dotdot
	@/bin/echo -n abcde > ../tests/filestores/compressed/compressed/..dotdot

	rm -rf ../tests/filestores/compressed/compressed/.dot
	@/bin/echo -n abcde > ../tests/filestores/compressed/compressed/.dot

	rm -rf ../tests/filestores/compressed/compressed/..dotdotdir
	mkdir ../tests/filestores/compressed/compressed/..dotdotdir

	rm -rf ../tests/filestores/compressed/compressed/.dotdir
	mkdir ../tests/filestores/compressed/compressed/.dotdir

	rm -f ../tests/filestores/compressed/compressed/..dotdotdir/file.txt
	@/bin/echo -n abcde > ../tests/filestores/compressed/compressed/..dotdotdir/file.txt

	rm -f ../tests/filestores/compressed/compressed/.dotdir/file.txt
	@/bin/echo -n abcde > ../tests/filestores/compressed/compressed/.dotdir/file.txt

	rm -f ../tests/filestores/webvfs/..dotdot
	@/bin/echo -n abcde > ../tests/filestores/webvfs/..dotdot

	rm -f ../tests/filestores/webvfs/.dot
	@/bin/echo -n abcde > ../tests/filestores/webvfs/.dot

	rm -rf ../tests/filestores/webvfs/..dotdotdir
	mkdir ../tests/filestores/webvfs/..dotdotdir

	rm -rf ../tests/filestores/webvfs/.dotdir
	mkdir ../tests/filestores/webvfs/.dotdir

	rm -f ../tests/filestores/webvfs/..dotdotdir/file.txt
	@/bin/echo -n abcde > ../tests/filestores/webvfs/..dotdotdir/file.txt

	rm -f ../tests/filestores/webvfs/.dotdir/file.txt
	@/bin/echo -n abcde > ../tests/filestores/webvfs/.dotdir/file.txt

	rm -f compressedvfsRootFSBackingStore.as LSOBackingStore.as *.abc *.swf ../framework/tests.swf
	rm -rf $(SHARED_OBJECTS_DIR)
	rm -f $(FLASHLOG)
	$(call nativepath,$(FLASCC)/usr/bin/genfs) ../tests/filestores/compressed compressedvfs
	cp $(call nativepath,$(FLASCC)/usr/share/LSOBackingStore.as) .

	$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-out VFS \
				LSOBackingStore.as compressedvfsRootFSBackingStore.as 	
				$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import VFS.abc \
				Console.as -outdir . -out Console

	$(call nativepath,$(FLASCC)/usr/bin/g++) setup.c VFS.abc $(call nativepath,$(FLASCC)/usr/lib/AlcVFSZip.abc) -symbol-abc=Console.abc -emit-swf -swf-size=1000x1000 -o setup.swf

	cp setup.swf ../framework/tests.swf
	$(PLAYER) ../framework/tests.swf
	sleep 2
	rm ../framework/tests.swf

