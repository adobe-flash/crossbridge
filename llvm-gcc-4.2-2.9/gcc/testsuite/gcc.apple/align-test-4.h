/* APPLE LOCAL file Macintosh alignment */

typedef struct {
     PEF_UBits32    symClass    :  8;
     PEF_UBits32    nameOffset  : 24;
     ByteCount      offset;
     SInt16         sectionNumber;
} NAME(LoaderExport, MODE);

typedef struct {
     unsigned    f1 : 32;
     char        f2;
} NAME(S1, MODE);

typedef struct {
     unsigned    f1;
     char        f2;
} NAME(S2, MODE);

typedef struct {
     char        f1;
     unsigned    f2 : 32;
     char        f3;
} NAME(S3, MODE);

typedef struct {
     char        f1;
     unsigned    f2_1 : 8;
     unsigned    f2_2 : 24;
     char        f3;
} NAME(S4, MODE);

typedef struct {
     char        f1;
     unsigned    f2 : 16;
     char        f3;
} NAME(S5, MODE);

typedef struct {
     char        f1;
     unsigned    f2 : 8;
     char        f3;
} NAME(S6, MODE);
