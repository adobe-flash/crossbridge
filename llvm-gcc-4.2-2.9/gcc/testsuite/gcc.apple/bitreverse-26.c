/* APPLE LOCAL file 4431496 */
extern void abort();
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=gnu99" } */
#pragma reverse_bitfields on
#pragma ms_struct on

#pragma pack(push,1)
typedef struct _S2
{
 unsigned char c ;
 unsigned short s1 : 9;
 unsigned short s2 : 7;
} S2;

#pragma pack(pop)

union U2 { S2 ss; int x[3]; };

int TestS2(void)
{
 union U2 u = {0};
 u.ss.c = 0x15;
 u.ss.s1 = 0x15;
 u.ss.s2 = 0x15;

 if (sizeof(S2) != 3 || u.x[0] != 0x152a1500)
    abort();
  return 0;
}

#pragma pack(push,2)
typedef struct _S3
{
 unsigned char c ;
 unsigned short s1 : 9;
 unsigned short s2 : 7;
} S3;

#pragma pack(pop)

union U3 { S3 ss; int x[3]; };

int TestS3(void)
{
 union U3 u = {0};
 u.ss.c = 0x15;
 u.ss.s1 = 0x15;
 u.ss.s2 = 0x15;

 if (sizeof(S3) != 4 || u.x[0] != 0x15002a15)
    abort();
  return 0;
}

typedef struct _S4
{
 unsigned char c ;
 unsigned short s1 : 9;
 unsigned short s2 : 7;
} S4;


union U4 { S4 ss; int x[3]; };

int TestS4(void)
{
 union U4 u = {0};
 u.ss.c = 0x15;
 u.ss.s1 = 0x15;
 u.ss.s2 = 0x15;

 if (sizeof(S4) != 4 || u.x[0] != 0x15002a15)
    abort();
  return 0;
}

int main()
{
 return TestS2() + TestS3() + TestS4();
}
