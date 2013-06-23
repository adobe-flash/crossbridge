 /* File PC1COD.c */ 
 /* written in Borland Turbo C 2.0 on PC */ 
 /* PC1 Cipher Algorithm ( Pukall Cipher 1 ) */ 
 /* By Alexander PUKALL 1991 */ 
 /* free code no restriction to use */ 
 /* please include the name of the Author in the final software */ 
 /* the Key is 256 bits */ 
 /* Tested with Turbo C 2.0 for DOS and Microsoft Visual C++ 5.0 for Win 32 */ 

 /* Note that PC1COD256.c is the encryption routine */ 
 /* PC1DEC256.c is the decryption routine */ 
 /* Only the K zone change in the two routines */ 
 /* You can create a single routine with the two parts in it */ 

#include <stdio.h>
#include <string.h>
unsigned short  ax, bx, cx, dx, si, tmp, x1a2, x1a0[16], res, i,
inter, cfc, cfd, compte;

unsigned char   cle[32];	/* les variables sont definies de facon
 * globale */

unsigned char   buff[32];

short           c;

int             c1, count;

short           d, e;

FILE * in;


assemble() 
{
  
  
  x1a0[0] = (cle[0] * 256) + cle[1];
  
  code();
  
  inter = res;
  
  
  x1a0[1] = x1a0[0] ^ ((cle[2] * 256) + cle[3]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[2] = x1a0[1] ^ ((cle[4] * 256) + cle[5]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[3] = x1a0[2] ^ ((cle[6] * 256) + cle[7]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[4] = x1a0[3] ^ ((cle[8] * 256) + cle[9]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[5] = x1a0[4] ^ ((cle[10] * 256) + cle[11]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[6] = x1a0[5] ^ ((cle[12] * 256) + cle[13]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[7] = x1a0[6] ^ ((cle[14] * 256) + cle[15]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[8] = x1a0[7] ^ ((cle[16] * 256) + cle[17]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[9] = x1a0[8] ^ ((cle[18] * 256) + cle[19]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[10] = x1a0[9] ^ ((cle[20] * 256) + cle[21]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[11] = x1a0[10] ^ ((cle[22] * 256) + cle[23]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[12] = x1a0[11] ^ ((cle[24] * 256) + cle[25]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[13] = x1a0[12] ^ ((cle[26] * 256) + cle[27]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[14] = x1a0[13] ^ ((cle[28] * 256) + cle[29]);
  
  code();
  
  inter = inter ^ res;
  
  
  x1a0[15] = x1a0[14] ^ ((cle[30] * 256) + cle[31]);
  
  code();
  
  inter = inter ^ res;
  
  
  i = 0;
  
  return (0);
  
} 

code() 
{
  
  dx = x1a2 + i;
  
  ax = x1a0[i];
  
  cx = 0x015a;
  
  bx = 0x4e35;
  
  
  tmp = ax;
  
  ax = si;
  
  si = tmp;
  
  
  tmp = ax;
  
  ax = dx;
  
  dx = tmp;
  
  
  if (ax != 0)
    
  {
    
    ax = ax * bx;
    
  } 
  
  tmp = ax;
  
  ax = cx;
  
  cx = tmp;
  
  
  if (ax != 0)
    
  {
    
    ax = ax * si;
    
    cx = ax + cx;
    
  } 
  
  tmp = ax;
  
  ax = si;
  
  si = tmp;
  
  ax = ax * bx;
  
  dx = cx + dx;
  
  
  ax = ax + 1;
  
  
  x1a2 = dx;
  
  x1a0[i] = ax;
  
  
  res = ax ^ dx;
  
  i = i + 1;
  
  return (0);
  
} 

/* Use Linear congruential PRNG */
int my_rand_r(int *seedp)
{
  /* Knuth & Lewis */
  unsigned x = *seedp * 1664525 + 1013904223;
  *seedp = x;
  return (x >> 16) & 0x7fff;
}

main(int argc, const char **argv) 
{
  int NumInput = 20000;
  int Print;
  int random_seed = 1;
  
  if (argc == 2) NumInput = atoi(argv[1]);
  
  si = 0;
  
  x1a2 = 0;
  
  i = 0;
  
  
  
  /* ('abcdefghijklmnopqrstuvwxyz012345') is the default password used */ 
  /* if the user enter a key < 32 characters, characters of the default */ 
  /* password will be used */ 
  
  memcpy(cle, "abcdefghijklmnopqrstuvwxyz012345", 32);
  
  
  printf("PC1 Cipher 256 bits \nENCRYPT file IN.BIN to OUT.BIN\n");
  
  
  /* hack so that it doesn't require user input. ntclark 3/12/04 */ 
  buff[1] = '\0';
  
  
  if (strlen(buff) > 32)
    
  {
    count = 32;
  } 
  else
    
  {
    count = strlen(buff);
  } 
  
  for (c1 = 0; c1 < count; c1++)
    
  {
    
    cle[c1] = buff[c1];
    
  } 
  
  Print = 0;
  while (--NumInput) {	
    c = my_rand_r(&random_seed);/* c contains the byte read in the file */
    assemble();
    
    cfc = inter >> 8;
    
    cfd = inter & 255;	/* cfc^cfd = random byte */
    
    
    /* K ZONE !!!!!!!!!!!!! */ 
    /*
     * here the mix of c and cle[compte] is before the encryption
     * of c
     */ 
    
    for (compte = 0; compte <= 31; compte++)
    {
      
      /* we mix the plaintext byte with the key */ 
      
      cle[compte] = cle[compte] ^ c;
      
    } 
    
    c = c ^ (cfc ^ cfd);
    
    
    
    d = (c >> 4);	/* we split the 'c' crypted byte into
     * two 4 bits parts 'd' and 'e' */
    
    e = (c & 15);
    
    if ((++ Print & 2047) == 0)
      printf("%d %d ", d, e);
  } 

  printf("\n");
  return (0);
} 

