// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <sys/stat.h>
#include <stdio.h>
#include <stddef.h>

/* This program prints the names, offsets, and sizes for a subset of the
 * members of the stat struct. The format of the output is appropriate for
 * use in a python program. Offset refers to the memory location of a
 * struct member relative to the location of the struct itself, as in
 * the offsetof macro.
 *
 * The output of this program is included in gensyscalls.py because it
 * needs to know the memory layout of struct stat on flascc. You
 * probably want to compile this code against flascc's SDK.
 */

#define ENTRY(x, y) print_entry(#x, offsetof(struct stat, x), sizeof(y))
#define TS_ENTRY(x) \
    { \
        print_entry(#x ".tv_sec", offsetof(struct stat, x) + \
            offsetof(struct timespec, tv_sec), sizeof(time_t)); \
        print_entry(#x ".tv_nsec", offsetof(struct stat, x) + \
            offsetof(struct timespec, tv_nsec), sizeof(long)); \
    } \

void print_entry(char const *name, size_t offset, size_t size) {
    printf("(\"%s\", %u, %u),\n", name, offset, size);
}

int main(int argc, char **argv) {
    printf("[");

    ENTRY(st_dev, __dev_t);
    ENTRY(st_mode, mode_t);
    ENTRY(st_nlink, nlink_t);
    ENTRY(st_uid, uid_t);
    ENTRY(st_gid, gid_t);
    ENTRY(st_rdev, __dev_t);
    ENTRY(st_size, off_t);
    ENTRY(st_blocks, blkcnt_t);
    ENTRY(st_blksize, blksize_t);
    TS_ENTRY(st_atimespec);
    TS_ENTRY(st_mtimespec);
    TS_ENTRY(st_ctimespec);

    printf("]\n");
    return 0;
}

