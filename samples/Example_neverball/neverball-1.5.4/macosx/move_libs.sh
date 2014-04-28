#!/bin/sh
# This shell script moves all files needed to the right directories EXCEPT SDLMain.h and SDLMain.m
# Get these from the /SDLMain/NIBless folder of the SDL devel extras found at http://www.libsdl.org/download-1.2.php.
# It will work only with MacPorts layout.
mkdir -p external_libs/vorbis external_libs/ogg
mkdir -p external_tools
cp /opt/local/lib/libpng12.a /opt/local/lib/libjpeg.a /opt/local/lib/libintl.a /opt/local/lib/libiconv.a /opt/local/lib/libvorbis.a /opt/local/lib/libvorbisenc.a /opt/local/lib/libvorbisfile.a /opt/local/lib/libogg.a /opt/local/lib/libz.a external_libs/.
cp /opt/local/include/libpng12/png.h /opt/local/include/libpng12/pngconf.h /opt/local/include/jpeglib.h /opt/local/include/jconfig.h /opt/local/include/jmorecfg.h /opt/local/include/jerror.h /opt/local/include/libintl.h /opt/local/include/iconv.h /opt/local/include/zconf.h /opt/local/include/zlib.h external_libs/.
cp /opt/local/include/vorbis/codec.h /opt/local/include/vorbis/vorbisenc.h /opt/local/include/vorbis/vorbisfile.h external_libs/vorbis/.
cp /opt/local/include/ogg/config_types.h /opt/local/include/ogg/ogg.h /opt/local/include/ogg/os_types.h external_libs/ogg/.
cp /opt/local/bin/msgfmt /opt/local/bin/seticon external_tools/.
