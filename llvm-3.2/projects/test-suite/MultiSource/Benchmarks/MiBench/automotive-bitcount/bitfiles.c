/* +++Date last modified: 05-Jul-1997 */

/*
**  BITFILES.C - reading/writing bit files
**
**  Public domain by Aare Tali
*/

#include <stdlib.h>
#include "bitops.h"

bfile *bfopen(char *name, char *mode)
{
      bfile * bf;

      bf = malloc(sizeof(bfile));
      if (NULL == bf)
            return NULL;
      bf->file = fopen(name, mode);
      if (NULL == bf->file)
      {
            free(bf);
            return NULL;
      }
      bf->rcnt = 0;
      bf->wcnt = 0;
      return bf;
}

int bfread(bfile *bf)
{
      if (0 == bf->rcnt)          /* read new byte */
      {
            bf->rbuf = (char)fgetc(bf->file);
            bf->rcnt = 8;
      }
      bf->rcnt--;
      return (bf->rbuf & (1 << bf->rcnt)) != 0;
}

void bfwrite(int bit, bfile *bf)
{
      if (8 == bf->wcnt)          /* write full byte */
      {
            fputc(bf->wbuf, bf->file);
            bf->wcnt = 0;
      }
      bf->wcnt++;
      bf->wbuf <<= 1;
      bf->wbuf |= bit & 1;
}

void bfclose(bfile *bf)
{
      fclose(bf->file);
      free(bf);
}

#ifdef TEST

void test1(void)
{
      bfile *out;
      bfile *in;
      FILE  *in1;
      FILE  *in2;

      in = bfopen("bitfiles.c", "rb");
      out = bfopen("bitfiles.cc", "wb");
      if ((NULL == in) || (NULL == out))
      {
            printf("Can't open/create test files\n");
            exit(1);
      }
      while (!feof(in->file))
            bfwrite(bfread(in), out);
      bfclose(in);
      bfclose(out);
      in1 = fopen("bitfiles.c", "rb");
      in2 = fopen("bitfiles.cc", "rb");
      if ((NULL == in1) || (NULL == in2))
      {
            printf("Can't open test files for verifying\n");
            exit(1);
      }
      while (!feof(in1) && !feof(in2))
      {
            if (fgetc(in1) != fgetc(in2))
            {
                  printf("Files not identical, copy failed!\n");
                  exit(1);
            }
      }
      if (!feof(in1) || !feof(in2))
      {
            printf("Not same size, copy failed!\n");
            exit(1);
      }
      fclose(in1);
      fclose(in2);
}

void test2(void)
{
      FILE  *in1;
      bfile *in2;
      int    ch;

      in1 = fopen("bitfiles.c", "rb");
      in2 = bfopen("bitfiles.cc", "rb");
      if ((NULL == in1) || (NULL == in2))
      {
            printf("Can't open test files\n");
            exit(1);
      }
      while (!feof(in1) && !feof(in2->file))
      {
            ch = fgetc(in1);
            if (ch < ' ')
                  ch = '.';
            printf(" '%c' ", ch);
            for (ch = 0; ch < 8; ch++)
                  printf("%c", "01"[bfread(in2)]);
            printf("   ");
      }
      fclose(in1);
      bfclose(in2);
}

main()
{
      test1();
      test2();
      return 0;
}

#endif /* TEST */
