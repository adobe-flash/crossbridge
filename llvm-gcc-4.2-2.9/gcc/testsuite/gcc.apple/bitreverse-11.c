/* APPLE LOCAL file */
/* Radar 4296479 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-w" } */

extern void abort(void);

#pragma reverse_bitfields on
#pragma ms_struct on

typedef struct
{
    unsigned short bits1:3, bits2:5, bits3:8;
} BFSTRUCT;

const BFSTRUCT bfInit = { 3, 0, 0 };

int main()
{
    if (bfInit.bits1 != 3) abort();
    return 0;
}
