/* APPLE LOCAL file 6515001 */
/* { dg-do compile { target i?86-*-* x86_64-*-* } } */
/* { scan-assembler-not "cmove" } */
typedef struct __NSSymbol* NSSymbol;

static struct {
  unsigned pad[2];
  unsigned n_value;
  NSSymbol realSymbol;
} sLastLookup;

void foo(void *);
void* NSAddressOfSymbol(NSSymbol symbol)
{
  if ( (void*)symbol == (void*)(&sLastLookup) )
    symbol = sLastLookup.realSymbol;

  foo(symbol);
  return symbol;
}
