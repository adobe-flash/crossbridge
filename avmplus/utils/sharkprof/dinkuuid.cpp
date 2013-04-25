/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
  Highly simple utility that sets the UUID of a mach-o object
  to a known value
*/

#include <stdio.h>
#include <stdlib.h>
#include <mach-o/loader.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s inOutFile\n", argv[0]);
        exit(-1);
    }

    FILE *f = fopen(argv[1], "r+");

    uint32_t ncmds = 0;

    mach_header header;

    fread(&header, sizeof(header), 1, f);

    if (header.magic == MH_MAGIC_64) {
        mach_header_64 header64;

        fseek(f, 0, SEEK_SET);
        fread(&header64, sizeof(header64), 1, f);
        ncmds = header64.ncmds;
    } else
        ncmds = header.ncmds;

    for (int n = 0; n < ncmds; n++) {
        load_command lc;

        fread(&lc, sizeof(lc), 1, f);

        if (lc.cmd == LC_UUID) {
            unsigned uuid[4] = { 'adob',
            'eado',
            'bead',
            'obe ' };

            unsigned ruuid[4];

            fread(&ruuid, sizeof(ruuid), 1, f);

            fseek(f, -sizeof(ruuid), SEEK_CUR);

            fwrite(&uuid, sizeof(uuid), 1, f);
            printf("found uuid %08x %08x %08x %08x\n", ruuid[0], ruuid[1],
                   ruuid[2], ruuid[3]);
        } else {
            fseek(f, lc.cmdsize - sizeof(lc), SEEK_CUR);
        }
    }
    fclose(f);
    return 0;
}
