/* APPLE LOCAL file 4182984 */
/* { dg-do run { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O3" } */
typedef unsigned int UINT;
typedef unsigned long UINT32;
typedef unsigned long BOOL32;
typedef signed int SINT;
typedef int INT;
typedef unsigned char UINT8;
typedef unsigned long UInt32;
typedef unsigned int uint32_t;
typedef unsigned long BitParse_T;
struct XX
{
 UINT8* dataEndPtr;
 UINT32* bufferCurrentPtr;
 UINT8 currentBitOffset;
 UINT32 currentBits;
};
typedef struct XX XX;
UINT32 ZZZ(UINT32 x) __attribute__((noinline));
UINT32 ZZZ(UINT32 x) { return x; }
void FlushUserData(XX* thiss) __attribute__((noinline));
void FlushUserData(XX* thiss)
{
 UINT ix;
 BOOL32 fNextBitsLookLikeStartCode;

 UINT32 currentBits;
 SINT currentBitOffset;
 UINT32* bufferCurrentPtr;

 while(1){
  { INT zNumBytesInBuffer; {;}; zNumBytesInBuffer=( ( ( (UINT8*)(thiss->dataEndPtr) ) - ( (UINT8*)(thiss->bufferCurrentPtr) ) ) -( (thiss->currentBitOffset+7)>>3 ) ); {;}; if(zNumBytesInBuffer<(1000)+4){ ; } };
  { currentBits =thiss->currentBits; currentBitOffset=thiss->currentBitOffset; bufferCurrentPtr=thiss->bufferCurrentPtr; };
  for(ix=0; ix<8; ix++){
   { UINT32 zData; { UINT zTmpOffset; zTmpOffset=(currentBitOffset)+(24); {;}; if(zTmpOffset<=32){ zData=(currentBits)>>(32 -(24)); }else{ UINT32 zTmpBits; zTmpBits=( (bufferCurrentPtr)[1] ); zTmpBits=((UInt32)(UInt32) (__builtin_constant_p(zTmpBits) ? ((((uint32_t)(zTmpBits) & 0xff000000) >> 24) | (((uint32_t)(zTmpBits) & 0x00ff0000) >> 8) | (((uint32_t)(zTmpBits) & 0x0000ff00) << 8) | (((uint32_t)(zTmpBits) & 0x000000ff) << 24)) : (__builtin_constant_p(zTmpBits) ? ((((uint32_t)(zTmpBits) & 0xff000000) >> 24) | (((uint32_t)(zTmpBits) & 0x00ff0000) >> 8) | (((uint32_t)(zTmpBits) & 0x0000ff00) << 8) | (((uint32_t)(zTmpBits) & 0x000000ff) << 24)) : ZZZ(zTmpBits)))); zData=((currentBits)>>(32 -(24))) |( zTmpBits >>(2*32 -zTmpOffset) ); } }; fNextBitsLookLikeStartCode=(zData==0x000001); };
   if(fNextBitsLookLikeStartCode){
    { thiss->currentBits =currentBits; thiss->currentBitOffset=currentBitOffset; thiss->bufferCurrentPtr=( (UINT32*)(bufferCurrentPtr) ); }; return;
   }
   { {;}; if( (currentBitOffset)<(32 -8) ){ (currentBitOffset) +=8; (currentBits )<<=8; }else{ BitParse_T zTmpBits; (currentBitOffset)-=(32 -8); zTmpBits=( *++(bufferCurrentPtr) ); zTmpBits=((UInt32)(UInt32) (__builtin_constant_p(zTmpBits) ? ((((uint32_t)(zTmpBits) & 0xff000000) >> 24) | (((uint32_t)(zTmpBits) & 0x00ff0000) >> 8) | (((uint32_t)(zTmpBits) & 0x0000ff00) << 8) | (((uint32_t)(zTmpBits) & 0x000000ff) << 24)) : (__builtin_constant_p(zTmpBits) ? ((((uint32_t)(zTmpBits) & 0xff000000) >> 24) | (((uint32_t)(zTmpBits) & 0x00ff0000) >> 8) | (((uint32_t)(zTmpBits) & 0x0000ff00) << 8) | (((uint32_t)(zTmpBits) & 0x000000ff) << 24)) : (zTmpBits)))); (currentBits) =zTmpBits<<(currentBitOffset); } };
  }
  { thiss->currentBits =currentBits; thiss->currentBitOffset=currentBitOffset; thiss->bufferCurrentPtr=( (UINT32*)(bufferCurrentPtr) ); };
 }
}

main() {
  UINT8 s[] = "abcd";
  UINT32 y;
  UINT32* q = &y;
  volatile struct XX p = { &s[0], q, 0, 0x100 };
  /* This is the normal calling sequence, with 0 placed into EAX after
     the stack is set up, so callee will crash if it uses EAX too soon. */
  asm("movl %0, (%%esp)\n\t movl $0, %%eax\n\tcall _FlushUserData"
	: : "r" (&p));
  return 0;
}
