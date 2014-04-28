#ifndef FS_PNG
#define FS_PNG

#include <png.h>

void fs_png_read (png_structp readp,  png_bytep data, png_size_t length);
void fs_png_write(png_structp writep, png_bytep data, png_size_t length);
void fs_png_flush(png_structp writep);

#endif
