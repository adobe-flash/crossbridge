#ifndef FS_JPG
#define FS_JPG

/*
 * jpeglib.h triggers errors due to missing size_t and FILE type
 * definitions.  Include these two headers as a workaround.
 */

#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>

#include "fs.h"

void fs_jpg_src(j_decompress_ptr cinfo, fs_file infile);

#endif
