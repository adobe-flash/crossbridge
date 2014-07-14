#ifndef FS_OV
#define FS_OV

#include <vorbis/vorbisfile.h>

size_t fs_ov_read(void *ptr, size_t size, size_t nmemb, void *datasource);
int    fs_ov_seek(void *datasource, ogg_int64_t offset, int whence);
int    fs_ov_close(void *datasource);
long   fs_ov_tell(void *datasource);

#endif
