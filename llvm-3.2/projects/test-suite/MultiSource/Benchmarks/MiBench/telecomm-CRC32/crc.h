/* +++Date last modified: 05-Jul-1997 */

/*
**  CRC.H - header file for SNIPPETS CRC and checksum functions
*/

#ifndef CRC__H
#define CRC__H

#include <stdlib.h>           /* For size_t                 */
#include "sniptype.h"         /* For BYTE, WORD, DWORD      */

/*
**  File: ARCCRC16.C
*/

void init_crc_table(void);
WORD crc_calc(WORD crc, char *buf, unsigned nbytes);
void do_file(char *fn);

/*
**  File: CRC-16.C
*/

WORD crc16(char *data_p, WORD length);

/*
**  File: CRC-16F.C
*/

WORD updcrc(WORD icrc, BYTE *icp, size_t icnt);

/*
**  File: CRC_32.C
*/

#define UPDC32(octet,crc) (crc_32_tab[((crc)^((BYTE)octet)) & 0xff] ^ ((crc) >> 8))

DWORD updateCRC32(unsigned char ch, DWORD crc);
Boolean_T crc32file(char *name, DWORD *crc, long *charcnt);
DWORD crc32buf(char *buf, size_t len);

/*
**  File: CHECKSUM.C
*/

unsigned checksum(void *buffer, size_t len, unsigned int seed);

/*
**  File: CHECKEXE.C
*/

void checkexe(char *fname);



#endif /* CRC__H */
