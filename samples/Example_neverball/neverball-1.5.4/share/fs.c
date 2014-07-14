#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <physfs.h>

#include "fs.h"
#include "dir.h"
#include "array.h"
#include "common.h"

/*
 * This file implements the virtual file system layer.  Most file
 * system and input/output operations are handled here.  There are
 * basically two groups of functions here: low-level functions
 * implemented directly using the PhysicsFS 1.0 API and higher-level
 * functions implemented using the former group.
 */

/* -------------------------------------------------------------------------- */

struct fs_file
{
    PHYSFS_file *handle;
};

int fs_init(const char *argv0)
{
    if (PHYSFS_init(argv0))
    {
        PHYSFS_permitSymbolicLinks(1);
        return 1;
    }

    return 0;
}

int fs_quit(void)
{
    return PHYSFS_deinit();
}

const char *fs_error(void)
{
    return PHYSFS_getLastError();
}

/* -------------------------------------------------------------------------- */

const char *fs_base_dir(void)
{
    return PHYSFS_getBaseDir();
}

int fs_add_path(const char *path)
{
    return PHYSFS_addToSearchPath(path, 0);
}

int fs_set_write_dir(const char *path)
{
    return PHYSFS_setWriteDir(path);
}

const char *fs_get_write_dir(void)
{
    return PHYSFS_getWriteDir();
}

/* -------------------------------------------------------------------------- */

Array fs_dir_scan(const char *path, int (*filter)(struct dir_item *))
{
    return dir_scan(path, filter, PHYSFS_enumerateFiles, PHYSFS_freeList);
}

void fs_dir_free(Array items)
{
    dir_free(items);
}

/* -------------------------------------------------------------------------- */

fs_file fs_open(const char *path, const char *mode)
{
    fs_file fh;

    assert((mode[0] == 'r' && !mode[1]) ||
           (mode[0] == 'w' && (!mode[1] || mode[1] == '+')));

    if ((fh = malloc(sizeof (*fh))))
    {
        switch (mode[0])
        {
        case 'r':
            fh->handle = PHYSFS_openRead(path);
            break;

        case 'w':
            fh->handle = (mode[1] == '+' ?
                          PHYSFS_openAppend(path) :
                          PHYSFS_openWrite(path));
            break;
        }

        if (fh->handle)
        {
            PHYSFS_setBuffer(fh->handle, 0x2000);
        }
        else
        {
            free(fh);
            fh = NULL;
        }
    }

    return fh;
}

int fs_close(fs_file fh)
{
    if (PHYSFS_close(fh->handle))
    {
        free(fh);
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */

int fs_mkdir(const char *path)
{
    return PHYSFS_mkdir(path);
}

int fs_exists(const char *path)
{
    return PHYSFS_exists(path);
}

int fs_remove(const char *path)
{
    return PHYSFS_delete(path);
}

/* -------------------------------------------------------------------------- */

int fs_read(void *data, int size, int count, fs_file fh)
{
    return PHYSFS_read(fh->handle, data, size, count);
}

int fs_write(const void *data, int size, int count, fs_file fh)
{
    return PHYSFS_write(fh->handle, data, size, count);
}

int fs_flush(fs_file fh)
{
    return PHYSFS_flush(fh->handle);
}

long fs_tell(fs_file fh)
{
    return PHYSFS_tell(fh->handle);
}

int fs_seek(fs_file fh, long offset, int whence)
{
    PHYSFS_uint64 pos = 0;
    PHYSFS_sint64 cur = PHYSFS_tell(fh->handle);
    PHYSFS_sint64 len = PHYSFS_fileLength(fh->handle);

    switch (whence)
    {
    case SEEK_SET:
        pos = offset;
        break;

    case SEEK_CUR:
        if (cur < 0)
            return -1;
        pos = cur + offset;
        break;

    case SEEK_END:
        if (len < 0)
            return -1;
        pos = len + offset;
        break;
    }

    return PHYSFS_seek(fh->handle, pos);
}

int fs_eof(fs_file fh)
{
    return PHYSFS_eof(fh->handle);
}

int fs_length(fs_file fh)
{
    return PHYSFS_fileLength(fh->handle);
}

/* -------------------------------------------------------------------------- */

/*
 * The code below does not use the PhysicsFS API.
 */

/* -------------------------------------------------------------------------- */

static int cmp_dir_items(const void *A, const void *B)
{
    const struct dir_item *a = A, *b = B;
    return strcmp(a->path, b->path);
}

static int is_archive(struct dir_item *item)
{
    return strcmp(item->path + strlen(item->path) - 4, ".zip") == 0;
}

static void add_archives(const char *path)
{
    Array archives;
    int i;

    if ((archives = dir_scan(path, is_archive, NULL, NULL)))
    {
        array_sort(archives, cmp_dir_items);

        for (i = 0; i < array_len(archives); i++)
            fs_add_path(DIR_ITEM_GET(archives, i)->path);

        dir_free(archives);
    }
}

int fs_add_path_with_archives(const char *path)
{
    add_archives(path);
    return fs_add_path(path);
}

/* -------------------------------------------------------------------------- */

int fs_rename(const char *src, const char *dst)
{
    const char *write_dir;
    char *real_src, *real_dst;
    int rc = 0;

    if ((write_dir = fs_get_write_dir()))
    {
        real_src = concat_string(write_dir, "/", src, NULL);
        real_dst = concat_string(write_dir, "/", dst, NULL);

        rc = file_rename(real_src, real_dst);

        free(real_src);
        free(real_dst);
    }

    return rc;
}

/* -------------------------------------------------------------------------- */

int fs_getc(fs_file fh)
{
    unsigned char c;

    if (fs_read(&c, 1, 1, fh) != 1)
        return -1;

    return (int) c;
}

int fs_putc(int c, fs_file fh)
{
    unsigned char b = (unsigned char) c;

    if (fs_write(&b, 1, 1, fh) != 1)
        return -1;

    return b;
}

int fs_puts(const char *src, fs_file fh)
{
    while (*src)
        if (fs_putc(*src++, fh) < 0)
            return -1;

    return 0;
}

char *fs_gets(char *dst, int count, fs_file fh)
{
    char *s = dst;
    int c;

    assert(dst);
    assert(count > 0);

    if (fs_eof(fh))
        return NULL;

    while (count > 1 && (c = fs_getc(fh)) >= 0)
    {
        count--;

        *s = c;

        /* Keep a newline and break. */

        if (*s == '\n')
        {
            s++;
            break;
        }

        /* Ignore carriage returns. */

        if (*s == '\r')
        {
            count++;
            s--;
        }

        s++;
    }

    *s = '\0';

    return dst;
}

/* -------------------------------------------------------------------------- */

/*
 * Write out a multiline string to a file with appropriately converted
 * linefeed characters.
 */
static int write_lines(const char *start, int length, fs_file fh)
{
#ifdef _WIN32
    static const char crlf[] = "\r\n";
#else
    static const char crlf[] = "\n";
#endif

    int total_written = 0;

    int datalen;
    int written;
    char *lf;

    while (total_written < length)
    {
        lf = strchr(start, '\n');

        datalen = lf ? (int) (lf - start) : length - total_written;
        written = fs_write(start, 1, datalen, fh);

        if (written < 0)
            break;

        total_written += written;

        if (written < datalen)
            break;

        if (lf)
        {
            if (fs_puts(crlf, fh) < 0)
                break;

            total_written += 1;
            start = lf + 1;
        }
    }

    return total_written;
}

/* -------------------------------------------------------------------------- */

/*
 * Trying to avoid defining a feature test macro for every platform by
 * declaring vsnprintf with the C99 signature.  This is probably bad.
 */

#include <stdio.h>
#include <stdarg.h>

extern int vsnprintf(char *, size_t, const char *, va_list);

int fs_printf(fs_file fh, const char *fmt, ...)
{
    char *buff;
    int len;

    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    if ((buff = malloc(len)))
    {
        int written;

        va_start(ap, fmt);
        vsnprintf(buff, len, fmt, ap);
        va_end(ap);

        /*
         * HACK.  This assumes fs_printf is always called with the
         * intention of writing text, and not arbitrary data.
         */

        written = write_lines(buff, strlen(buff), fh);

        free(buff);

        return written;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

void *fs_load(const char *path, int *datalen)
{
    fs_file fh;
    void *data;

    data = NULL;

    if ((fh = fs_open(path, "r")))
    {
        if ((*datalen = fs_length(fh)) > 0)
        {
            if ((data = malloc(*datalen)))
            {
                if (fs_read(data, *datalen, 1, fh) != 1)
                {
                    free(data);
                    data = NULL;
                    *datalen = 0;
                }
            }
        }

        fs_close(fh);
    }

    return data;
}

/* -------------------------------------------------------------------------- */
