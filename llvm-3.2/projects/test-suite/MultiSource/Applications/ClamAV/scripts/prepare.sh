#!/bin/sh
BASEURL=http://svn.clamav.net/svn/clamav-devel/tags/clamav-0.92/
if test $# != 1; then
	echo "Usage: $0 <checkout|update|copy>"
	exit 1
fi
if [ $1 = "checkout" ]; then
	mkdir svn &&
	echo "Checking out ClamAV from $BASEURL ... to directory svn" &&
	cd svn && svn co $BASEURL/clamscan $BASEURL/libclamav $BASEURL/shared $BASEURL/test . &&
	svn cat $BASEURL/COPYING >./COPYING &&
	cd .. &&
	echo "Checkout complete" &&
	sh remove.sh &&
	echo "done"
elif [ $1 = "update" ]; then
	echo "svn up" &&
	svn up svn/clamscan svn/libclamav svn/shared svn/test &&
	sh remove.sh &&
	echo "done"
elif [ $1 = "getdb" ]; then
    	wget http://database.clamav.net/main.cvd http://database.clamav.net/daily.cvd -P dbdir/ 
elif [ $1 = "copy" ]; then
	echo "Copying&renaming files ..." &&
# special handling for clamscan/others.h, because we also have a libclamav/others.h
	mv svn/clamscan/others.h clamscan_others.h &&
	find svn -name '*.c' -exec sh rename.sh {} . \; &&
	find svn -name '*.h' -exec sh header_rename.sh {} . \; &&
	for i in clamscan_*.c; do
		sed -ie "s/\"others.h\"/\"clamscan_others.h\"/" $i;
	done &&
#flatten hierarchy	
	for i in *.[ch]; do
		sed -re 's/^(#include *")[^/]+\/([^"]+")/\1\2/g' -i $i;
	done &&
	cp svn/COPYING . &&
	#special case, regexec.c #includes a .c file directly, preserve name 
	mv libclamav_regex_engine.c engine.c &&
	sed -re 's/MSGCODE\("LibClamAV debug: "\)/puts\(str\)/g' -i libclamav_others.c &&
	touch target.h &&
	mkdir -p inputs &&
	cp svn/test/* inputs/ 
	echo "All files copied, you can now safely remove the directory svn" 
	#    clamscan/ files must include others.h from clamscan/ and not from libclamav unless explicitly specified in #include!	
fi

