#include "fs.h"
#include "fs_ov.h"

size_t fs_ov_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    return fs_read(ptr, size, nmemb, datasource);
}

int fs_ov_seek(void *datasource, ogg_int64_t offset, int whence)
{
    return fs_seek(datasource, offset, whence);
}

int fs_ov_close(void *datasource)
{
    return fs_close(datasource);
}

long fs_ov_tell(void *datasource)
{
    return fs_tell(datasource);
}
