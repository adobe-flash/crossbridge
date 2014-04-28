#include "fs_rwops.h"

static int rwops_seek(SDL_RWops *ctx, int offset, int whence)
{
    fs_file fh = ctx->hidden.unknown.data1;
    return fs_seek(fh, offset, whence) ? fs_tell(fh) : -1;
}

static int rwops_read(SDL_RWops *ctx, void *ptr, int size, int maxnum)
{
    return fs_read(ptr, size, maxnum, ctx->hidden.unknown.data1);
}

static int rwops_write(SDL_RWops *ctx, const void *ptr, int size, int num)
{
    return fs_write(ptr, size, num, ctx->hidden.unknown.data1);
}

static int rwops_close(SDL_RWops *ctx)
{
    fs_file fh = ctx->hidden.unknown.data1;

    if (!fs_close(fh))
        return -1;

    SDL_FreeRW(ctx);
    return 0;
}

SDL_RWops *fs_rwops_make(fs_file fh)
{
    SDL_RWops *ctx;

    if ((ctx = SDL_AllocRW()))
    {
        ctx->seek  = rwops_seek;
        ctx->read  = rwops_read;
        ctx->write = rwops_write;
        ctx->close = rwops_close;

        ctx->hidden.unknown.data1 = fh;
    }

    return ctx;
}

SDL_RWops *fs_rwops_open(const char *path, const char *mode)
{
    fs_file fh;

    if ((fh = fs_open(path, mode)))
        return fs_rwops_make(fh);

    return NULL;
}
