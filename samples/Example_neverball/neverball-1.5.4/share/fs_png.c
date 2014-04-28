#include <png.h>
#include <string.h>
#include "fs_png.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

void fs_png_read(png_structp readp, png_bytep data, png_size_t length)
{
    int read = fs_read(data, 1, length, png_get_io_ptr(readp));

    if (read < length)
        memset(data + read, 0, length - read);
}

void fs_png_write(png_structp writep, png_bytep data, png_size_t length)
{
    fs_write(data, 1, length, png_get_io_ptr(writep));
}

void fs_png_flush(png_structp writep)
{
    fs_flush(png_get_io_ptr(writep));
}

/*---------------------------------------------------------------------------*/
