#!/bin/sh
echo "Removing unnecessary files" &&
	pwd &&
	rm -rf svn/libclamav/js svn/libclamav/pst.c &&
	cat >svn/libclamav/pst.c <<EOF
#include "clamav-config.h"
#include "clamav.h"
int
cli_pst(const char *dir, int desc)
{
        cli_warnmsg("PST files not yet supported\n");
        return CL_EFORMAT;
}
EOF
# pst.c is huge, and is experimental, don't compile it
