#ifndef FS_RWOPS_H
#define FS_RWOPS_H

#include <SDL_rwops.h>
#include "fs.h"

SDL_RWops *fs_rwops_make(fs_file);
SDL_RWops *fs_rwops_open(const char *path, const char *mode);

#endif
