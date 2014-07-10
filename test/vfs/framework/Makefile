AS3COMP=java -jar $(call nativepath,$(FLASCC)/usr/lib/asc2.jar) -md -merge -abcfuture -AS3
$?SRCROOT=$(PWD)/../../..
$?UNAME=$(shell uname -s)

# This section is for Host platform options
ifneq (,$(findstring CYGWIN,$(UNAME)))
	$?nativepath=$(shell cygpath -at mixed $(1))
	$?SHARED_OBJECTS_DIR="$(USERPROFILE)/AppData/Roaming/Macromedia/Flash Player/\#SharedObjects"
else
	$?nativepath=$(abspath $(1))
	$?SHARED_OBJECTS_DIR="$(HOME)/Library/Preferences/Macromedia/Flash Player/#SharedObjects/"
endif

CFILES = ../tests/c/vfs_fflush.c ../tests/c/vfs_fwrite.c ../tests/c/vfs_readdir.c ../tests/c/vfs_fgetpos.c ../tests/c/vfs_rename.c ../tests/c/vfs_fopen.c ../tests/c/vfs_freopen.c ../tests/c/vfs_lseek.c ../tests/c/vfs_rewind.c ../tests/c/vfs_close.c ../tests/c/vfs_fputc.c ../tests/c/vfs_rmdir.c ../tests/c/vfs_closedir.c	../tests/c/vfs_fputs.c ../tests/c/vfs_mkdir.c ../tests/c/vfs_setbuf.c ../tests/c/vfs_fread.c ../tests/c/vfs_open.c ../tests/c/vfs_stat.c ../tests/c/vfs_fclose.c ../tests/c/vfs_fseek.c ../tests/c/vfs_opendir.c 	../tests/c/vfs_unlink.c ../tests/c/vfs_fcntl.c ../tests/c/vfs_printf.c ../tests/c/vfs_write.c ../tests/c/vfs_ferror.c ../tests/c/vfs_fstat.c ../tests/c/vfs_read.c ../tests/c/vfs_clock_gettime.c ../tests/c/vfs_ofstream.c ../tests/c/vfs_ifstream.c ../tests/c/vfs_fsetgetpos.c ../tests/c/vfs_getDirEntries.c ../tests/c/vfs_compressed.c ../tests/c/vfs_compressed_custom_package.c ../tests/c/vfs_embedded.c ../tests/c/vfs_http.c ../tests/c/vfs_lso.c ../tests/c/vfs_telldir.c

build:
	rm -r -f swcfilesystem.swc compressedvfsswcRootFSBackingStore.as compressedvfsbar.as embeddedvfs* compressedvfsRootFSBackingStore.as LSOBackingStore.as webvfs* manifest.as HttpBackingStore.as rm *.abc rm *.swf ReadOnly_*
	cd ../setup && make FLASCC=$(FLASCC)
	
	# Embedded file system
	$(call nativepath,$(FLASCC)/usr/bin/genfs) ../tests/filestores/embedded embeddedvfs --type=embed --name=EmbeddedVFS

	# Embedded read only file system
	rm -r -f ../tests/filestores/embedded_readonly/dir
	mkdir ../tests/filestores/embedded_readonly/dir
	$(call nativepath,$(FLASCC)/usr/bin/genfs) ../tests/filestores/embedded_readonly embeddedvfs_readonly --type=embed --read-only --name=EmbeddedVFSReadOnly

	# Compressed file system
	$(call nativepath,$(FLASCC)/usr/bin/genfs) ../tests/filestores/compressed compressedvfs

	# Compressed file system with package specified by genfs
	$(call nativepath,$(FLASCC)/usr/bin/genfs) ../tests/filestores/compressed compressedvfs --name=com.foo.bar

	# Compressed read only file system
	# This directory might contain junk
	rm -r -f ../tests/filestores/compressed_readonly/dir
	mkdir ../tests/filestores/compressed_readonly/dir
	$(call nativepath,$(FLASCC)/usr/bin/genfs) --read-only ../tests/filestores/compressed_readonly ReadOnly_ --name=ReadOnlyCompressedVFS

	# Compressed file system in a SWC
	$(call nativepath,$(FLASCC)/usr/bin/genfs) ../tests/filestores/compressed_swc compressedvfsswc

	# Web file system
	cp $(call nativepath,$(FLASCC)/usr/share/HTTPBackingStore.as) .
	$(call nativepath,$(FLASCC)/usr/bin/genfs) --type=http ../tests/filestores/webvfs webvfs
	mv webvfs_manifest.as manifest.as

	# Web read only file system
	# We do this in a subdirectory because manifest.as has to be called manifest.as, and we don't want to step on the earlier one.
	# Also, we have to rename HttpBackingStore because the HttpBackingStore class has already been defined earlier.
	mkdir webvfs_readonly
	cp $(call nativepath,$(FLASCC)/usr/share/HTTPBackingStore.as) webvfs_readonly/HTTPBackingStoreReadOnly.as
	sed -e 's/HTTPBackingStore/HTTPBackingStoreReadOnly/g' webvfs_readonly/HTTPBackingStoreReadOnly.as > webvfs_readonly/tmp
	mv webvfs_readonly/tmp webvfs_readonly/HTTPBackingStoreReadOnly.as
	# Now call genfs
	rm -r -f ../tests/filestores/webvfs_readonly/dir
	mkdir ../tests/filestores/webvfs_readonly/dir
	cd webvfs_readonly && $(call nativepath,$(FLASCC)/usr/bin/genfs) --type=http --read-only ../../tests/filestores/webvfs_readonly webvfs_readonly
	mv webvfs_readonly/webvfs_readonly_manifest.as webvfs_readonly/manifest.as
	cp webvfs_readonly/webvfs_readonly* .

	# LSO file system
	cp $(call nativepath,$(FLASCC)/usr/share/LSOBackingStore.as) .

	# Build TestInfo.abc and Util.abc
	$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				../tests/as/TestInfo.as -outdir .

	$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import TestInfo.abc \
				../tests/as/Util.as -outdir .

	# Build the main filesystem abc file and the Actionscript tests
	$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import TestInfo.abc \
				-import Util.abc \
				../tests/as/AlcVFS_addFile.as \
				../tests/as/AlcVFS_addDirectory.as \
				../tests/as/AlcVFS_deleteFile.as \
				../tests/as/AlcVFS_getDirEntries.as \
				../tests/as/AlcVFS_getFileHandleFromPath.as \
				../tests/as/AlcVFS_openFile.as \
				compressedvfsRootFSBackingstore.as \
				compressedvfsbar.as \
				ReadOnly_ReadOnlyCompressedVFS.as \
				embeddedvfs_0000*.as \
				embeddedvfs_readonly_0000*.as \
				LSOBackingStore.as \
				HTTPBackingStore.as \
				webvfs_readonly/HTTPBackingStoreReadOnly.as \
				-outdir . -out VFS

	# Build swcfilesystem.abc
	$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/AlcVFSZip.abc) \
				compressedvfsswcRootFSBackingStore.as -outdir . -out swcfilesystem

	# Build the console
	$(AS3COMP)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import TestInfo.abc \
				-import VFS.abc \
				-import HTTPBackingStore.as \
				Console.as -outdir . -out Console

	# Make the first SWF (contains most of the tests)
	$(call nativepath,$(FLASCC)/usr/bin/g++) testing_shell.c $(CFILES) TestInfo.abc Util.abc VFS.abc $(FLASCC)/usr/lib/AlcVFSZip.abc -symbol-abc=Console.abc -emit-swf -swf-size=1000x1000 -o tests.swf

	# Make the filesystem.swc and swf files.
	$(call nativepath,$(FLASCC)/usr/bin/g++) -O3 swcmain.c swcfilesystem.abc $(call nativepath,$(FLASCC)/usr/lib/AlcVFSZip.abc) -emit-swc=bar -o swcfilesystem.swc
	$(call nativepath,$(FLEX)/bin/mxmlc) -library-path=swcfilesystem.swc -debug=true SwcConsole.as -o swctests.swf
