/* APPLE LOCAL file 4158356 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-O2" } */
struct CGFontSharedCacheKey {
    unsigned int gfid:16;
    unsigned int glyph:16;
    unsigned int m1:7;		/* Values from -64 to 63. */
    unsigned int m2:7;		/* Values from -64 to 63. */
    unsigned int smoothing:3;
    unsigned int unused:6;
    unsigned int mode:3;
    unsigned int qx:3;		/* Values from 0 to 7. */
    unsigned int qy:3;		/* Values from 0 to 7. */
};
typedef struct CGFontSharedCacheKey CGFontSharedCacheKey;


void
CGFontKeyGetSharedCacheKeys(CGFontSharedCacheKey *dst, const CGFontSharedCacheKey *src)
{
	*dst = *src;
}
/* { dg-final { scan-assembler-not "andb" } } */
