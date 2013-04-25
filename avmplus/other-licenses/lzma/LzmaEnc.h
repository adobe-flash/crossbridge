/*  LzmaEnc.h -- LZMA Encoder
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __LZMA_ENC_H
#define __LZMA_ENC_H

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LZMA_PROPS_SIZE 5

typedef struct _CLzmaEncProps
{
  int level;       /*  0 <= level <= 9 */
  UInt32 dictSize; /* (1 << 12) <= dictSize <= (1 << 27) for 32-bit version
                      (1 << 12) <= dictSize <= (1 << 30) for 64-bit version
                       default = (1 << 24) */
  int lc;          /* 0 <= lc <= 8, default = 3 */
  int lp;          /* 0 <= lp <= 4, default = 0 */
  int pb;          /* 0 <= pb <= 4, default = 2 */
  int algo;        /* 0 - fast, 1 - normal, default = 1 */
  int fb;          /* 5 <= fb <= 273, default = 32 */
  int btMode;      /* 0 - hashChain Mode, 1 - binTree mode - normal, default = 1 */
  int numHashBytes; /* 2, 3 or 4, default = 4 */
  UInt32 mc;        /* 1 <= mc <= (1 << 30), default = 32 */
  unsigned writeEndMark;  /* 0 - do not write EOPM, 1 - write EOPM, default = 0 */
  int numThreads;  /* 1 or 2, default = 2 */
} CLzmaEncProps;

void LzmaEncProps_Init(CLzmaEncProps *p);
void LzmaEncProps_Normalize(CLzmaEncProps *p);
UInt32 LzmaEncProps_GetDictSize(const CLzmaEncProps *props2);


/* ---------- CLzmaEncHandle Interface ---------- */

/* LzmaEnc_* functions can return the following exit codes:
Returns:
  SZ_OK           - OK
  SZ_ERROR_MEM    - Memory allocation error
  SZ_ERROR_PARAM  - Incorrect paramater in props
  SZ_ERROR_WRITE  - Write callback error.
  SZ_ERROR_PROGRESS - some break from progress callback
  SZ_ERROR_THREAD - errors in multithreading functions (only for Mt version)
*/

typedef void * CLzmaEncHandle;

CLzmaEncHandle LzmaEnc_Create(ISzAlloc *alloc);
void LzmaEnc_Destroy(CLzmaEncHandle p, ISzAlloc *alloc, ISzAlloc *allocBig);
SRes LzmaEnc_SetProps(CLzmaEncHandle p, const CLzmaEncProps *props);
SRes LzmaEnc_WriteProperties(CLzmaEncHandle p, Byte *properties, SizeT *size);
SRes LzmaEnc_Encode(CLzmaEncHandle p, ISeqOutStream *outStream, ISeqInStream *inStream,
    ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);
SRes LzmaEnc_MemEncode(CLzmaEncHandle p, Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
    int writeEndMark, ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);

typedef struct
{
  ISeqOutStream funcTable;
  Byte *data;
  SizeT rem;
  LzmaBool overflow;
  void* userdata;  //this can be used for passing interface or data structure for write function
} CSeqOutStreamBuf;

/*
  p    : pointer of CSeqOutStreamBuf 
  buf  : encoded byte data buffer
  size : encoded byte data size
*/
typedef  size_t (*WriteFunc)(void *p, const void *buf, size_t size);

/*
In:
	   UserWrite      - write function pointer which will be called by encoder
       UserData       - user defined data pointer which will be passed to write function as first parameter p (CSeqOutStreamBuf.userdata)       
       src                - input data
       srcLen           - input data size
       props            - LZMA properties (5 bytes) format
       writeEndMark - encoded data end mark
       progress        - ICompressProgress function pointer
       alloc              - allocator
       allocBig         - allocator


Out:     
 	   destLen         - encoded data size which will be set by encoder when encoding is finished
       propsEncoded - encoded LZMA properties
       propsSize       - encoded LZMA properties size

Returns:
       SZ_OK                - OK
       SZ_ERROR_DATA        - Data error
       SZ_ERROR_MEM         - Memory allocation arror
       SZ_ERROR_UNSUPPORTED - Unsupported properties
       SZ_ERROR_INPUT_EOF   - it needs more bytes in input buffer (src)
*/

SRes LzmaDynamicEncode(WriteFunc   UserWrite, void* DestData, SizeT *destLen, const Byte *src, SizeT srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, SizeT *propsSize, int writeEndMark,
    ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);
    
/* ---------- One Call Interface ---------- */

/* LzmaEncode
Return code:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
  SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
*/

SRes LzmaEncode(Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, SizeT *propsSize, int writeEndMark,
    ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);

#ifdef __cplusplus
}
#endif

#endif
