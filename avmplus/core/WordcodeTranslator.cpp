/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
#ifdef SUPERWORD_PROFILING
    // 32-bit only

    static FILE *swprof_code_fp = NULL;
    static FILE *swprof_sample_fp = NULL;
#ifdef SUPERWORD_LIMIT
    static unsigned int sample_count = 0;
#endif

    void WordcodeTranslator::swprofStart()
    {
        swprof_code_fp = fopen("superwordprof_code.txt", "wb");
        if (swprof_code_fp == NULL)
            AvmLog("SUPERWORD PROFILING: COULD NOT OPEN CODE FILE.\n");
        else
        {
            unsigned int signature = 0xC0DEC0DE;
            fwrite(&signature, sizeof(uint32_t), 1, swprof_code_fp);
        }
        swprof_sample_fp = fopen("superwordprof_sample.txt", "wb");
        if (swprof_sample_fp == NULL)
            AvmLog("SUPERWORD PROFILING: COULD NOT OPEN SAMPLE FILE.\n");
        else
        {
            unsigned int signature = 0xDA1ADA1A;
            fwrite(&signature, sizeof(uint32_t), 1, swprof_sample_fp);
        }
    }

    void WordcodeTranslator::swprofStop()
    {
        if (swprof_code_fp != NULL) { fclose(swprof_code_fp); swprof_code_fp = NULL; }
        if (swprof_sample_fp != NULL) { fclose(swprof_sample_fp); swprof_sample_fp = NULL; }
    }

    void WordcodeTranslator::swprofCode(const uint32_t* start, const uint32_t* limit)
    {
        if (swprof_code_fp != NULL) {
            fwrite(&start, sizeof(uint32_t*), 1, swprof_code_fp);
            fwrite(&limit, sizeof(uint32_t*), 1, swprof_code_fp);
            fwrite(start, sizeof(uint32_t), limit-start, swprof_code_fp);
            fflush(swprof_code_fp);
        }
    }

    void WordcodeTranslator::swprofPC(const uint32_t* pc)
    {
        if (swprof_sample_fp != NULL) {
            fwrite(&pc, sizeof(uint32_t*), 1, swprof_sample_fp);
#ifdef SUPERWORD_LIMIT
            if (++sample_count == SUPERWORD_LIMIT) {
                swprofStop();
                AvmLog("SAMPLING HALTED.\n");
            }
#endif
        }
    }
#endif  // SUPERWORD_PROFILING
}
