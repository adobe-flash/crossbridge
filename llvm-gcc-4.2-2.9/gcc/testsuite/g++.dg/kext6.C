/* APPLE LOCAL file KEXT indirect-virtual-calls --sts */
/* Radar 3848842 Positive C++ test case. */
/* Origin F. Jahanian <fjahanian@apple.com> */
/* { dg-do compile } */
/* { dg-options "-Os  -fapple-kext" } */

struct Integer
{
 Integer ( void ) ;
 virtual ~Integer ( void ) ;
};

static void AppendMa(unsigned char *outBuffer);
static void AppendNonce();

extern "C" void afpfs_DHXLogin ()
{
    unsigned char *cptr = __null;
    AppendMa(cptr); 
    AppendNonce(); 
}

static void AppendNonce() { }


static void AppendMa(unsigned char *outBuffer)
{
 Integer Ma;
}
