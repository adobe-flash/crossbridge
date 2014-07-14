#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* salsa20 is due to Daniel J. Bernstein, who has stated
   "My policy is that Salsa20 is free for everyone to use."
   ( http://cr.yp.to/snuffle/ip.pdf )
*/

uint32_t STATE[16];
uint32_t outbuf[16];  
uint32_t ptr=0;

#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
void salsa20(uint32_t out[16],uint32_t in[16]) {
  uint32_t i, x[16];

  for (i = 0;i < 16;++i)
    x[i] = in[i];
    
  for (i = 20;i > 0;i -= 2) {
    x[ 4] ^= R(x[ 0]+x[12], 7);  x[ 8] ^= R(x[ 4]+x[ 0], 9);
    x[12] ^= R(x[ 8]+x[ 4],13);  x[ 0] ^= R(x[12]+x[ 8],18);
    x[ 9] ^= R(x[ 5]+x[ 1], 7);  x[13] ^= R(x[ 9]+x[ 5], 9);
    x[ 1] ^= R(x[13]+x[ 9],13);  x[ 5] ^= R(x[ 1]+x[13],18);
    x[14] ^= R(x[10]+x[ 6], 7);  x[ 2] ^= R(x[14]+x[10], 9);
    x[ 6] ^= R(x[ 2]+x[14],13);  x[10] ^= R(x[ 6]+x[ 2],18);
    x[ 3] ^= R(x[15]+x[11], 7);  x[ 7] ^= R(x[ 3]+x[15], 9);
    x[11] ^= R(x[ 7]+x[ 3],13);  x[15] ^= R(x[11]+x[ 7],18);
    x[ 1] ^= R(x[ 0]+x[ 3], 7);  x[ 2] ^= R(x[ 1]+x[ 0], 9);
    x[ 3] ^= R(x[ 2]+x[ 1],13);  x[ 0] ^= R(x[ 3]+x[ 2],18);
    x[ 6] ^= R(x[ 5]+x[ 4], 7);  x[ 7] ^= R(x[ 6]+x[ 5], 9);
    x[ 4] ^= R(x[ 7]+x[ 6],13);  x[ 5] ^= R(x[ 4]+x[ 7],18);
    x[11] ^= R(x[10]+x[ 9], 7);  x[ 8] ^= R(x[11]+x[10], 9);
    x[ 9] ^= R(x[ 8]+x[11],13);  x[10] ^= R(x[ 9]+x[ 8],18);
    x[12] ^= R(x[15]+x[14], 7);  x[13] ^= R(x[12]+x[15], 9);
    x[14] ^= R(x[13]+x[12],13);  x[15] ^= R(x[14]+x[13],18);
  }
  for (i = 0;i < 16;++i) out[i] = x[i] + in[i];
}

uint32_t salsa(void) {
  if(ptr==0) {
    salsa20(outbuf, STATE);
    (uint64_t)STATE[0]++;
  }
  
  ptr = (ptr+1) & 0xF;
  return(outbuf[ptr]);
}

int main(void) {
  uint32_t val, i;
#ifdef SMALL_PROBLEM_SIZE
  uint32_t count = 5379194;
  uint32_t offset = 0xf655703d - 0xb67e7950;
#else
  uint32_t count = 537919488;
  uint32_t offset = 0;
#endif

  for(i=0; i<16; i++)
    STATE[i] = (0xedababe5+(i+13))^(0xdeadbeef-i);
  
  for(i=0; i<count; i++)
    salsa();

  val = salsa();
  val += offset;

  printf("got:       %x\n", val);
  printf("should be: %x\n", 0xf655703d);
  return(val != 0xf655703d);
}

