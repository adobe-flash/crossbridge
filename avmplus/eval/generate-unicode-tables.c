/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Generate simple lookup tables from the unicode table.
 * Below, 'UnicodeData.txt' is the data file downloaded from unicode.org.
 *
 * Compile this program:
 *   cc -o ranges ranges.c
 *
 * Extract the desired data rows:
 *   egrep ';(Lu|Ll|Lt|Lm|Lo|Nl);' UnicodeData.txt > IdentifierStart.txt
 *
 * Generate data definitions:
 *   ranges IdentifierStart.txt identifier_start > identifier_start.h
 *
 * All character values in the range above 0xFFFF are ignored.
 *
 * The table encodings are straightforward.  It's possible to code the
 * tables more efficiently if the high value of a range can be a delta
 * to the low value, and if the low value of the next range can be a
 * delta to the high of the previous range.  For the identifier_start
 * table the storage can be cut in half, pretty much.  The function
 * 'analyze()' computes statistics for these.  It should also be
 * possible to store the singleton table as delta values, but I've not
 * examined this.
 */

#include <stdio.h>
#include <string.h>

struct { int lo, hi; } ranges[10000];
int next_range = 0;

int singletons[10000];
int next_singleton = 0;

int exceptions[10000];
int next_exception = 0;

void enter(int lo, int hi)
{
    if (lo < hi) {
        ranges[next_range].lo = lo;
        ranges[next_range].hi = hi;
        next_range++;
    }
    else
        singletons[next_singleton++] = lo;
}

/* This analysis tries to merge ranges that are close together by
 * placing the values that aren't in the merged range into an
 * exception table.  It didn't pay off for ECMAScript identifier data;
 * no ranges were merged with the criteria used here.
 */
/*
void merge()
{
    int i=0, j=0, k;
    while (j < next_range) {
        if (ranges[j].lo - ranges[i].hi <= 3) {
            for ( k=ranges[i].hi+1 ; k <= ranges[j].lo-1 ; k++ )
                exceptions[next_exception++] = k;
            ranges[i].hi = ranges[j].hi;
            ranges[j].lo = ranges[j].hi = -2;
            j++;
        }
        else
            i = ++j;
    }
}
*/

void analyze()
{
    int i, j;
    int wide = 0;
    int wide_gap = 0;
    
    for ( i=0 ; i < next_range ; i++ )
        if (ranges[i].hi - ranges[i].lo > 254)
            wide++;
    printf("Narrow ranges: %d\n", next_range - wide);
    printf("Wide ranges: %d\n", wide);
    for ( i=1 ; i < next_range ; i++ )
        if (ranges[i-1].hi - ranges[i].lo > 255)
            wide_gap++;
    printf("Narrow gaps: %d\n", (next_range - 1) - wide_gap);
    printf("Wide gaps: %d\n", wide_gap);
}

void print(const char* prefix)
{
    int i, numranges=0;
    
    for ( i=0 ; i < next_range ; i++ )
        if (ranges[i].lo != -2)
            ++numranges;
    
    printf("static const uint16_t %s_ranges[][2] = {\n", prefix);
    for ( i=0 ; i < next_range ; i++ )
        if (ranges[i].lo != -2)
            printf("{0x%04X, 0x%04X},\n", ranges[i].lo, ranges[i].hi);
    printf("};\n\n");
    
    printf("static const uint16_t %s_singletons[] = {\n", prefix);
    for ( i=0 ; i < next_singleton; i++ )
        printf("0x%04X,\n", singletons[i]);
    printf("};\n\n");
    
    printf("static const unicode_table_t %s = {\n", prefix);
    printf("    %d,\n", numranges);
    printf("    %s_ranges,\n", prefix);
    printf("    %d,\n", next_singleton);
    printf("    %s_singletons\n", prefix);
    printf("};\n");
}

int main(int argc, char** argv)
{
    char buf[500];
    
    if (argc != 3)
        goto fail;
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
        goto fail;
    
    int lo, hi;
    hi = -2;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        int n;
        buf[sizeof(buf)-1] = 0;
        if (strlen(buf) == sizeof(buf)-1)
            goto fail;
        if (sscanf(buf, "%x;", &n) != 1)
            goto fail;
        if (n > 0xFFFF)
            break;
        if (n == hi+1)
            hi = n;
        else {
            if (hi != -2) 
                enter(lo, hi);
            lo=hi=n;
        }
    }
    if (hi != -2)
        enter(lo, hi);
    
    fclose(fp);
    
    //merge();
    //analyze();
    print(argv[2]);
    
    return 0;
    
fail:
    printf("Failure.\n");
    return 1;
}
