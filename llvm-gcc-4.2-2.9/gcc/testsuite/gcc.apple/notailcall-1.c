/* APPLE LOCAL file Radar 3830232 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not "\(jmp|b\)\[ \\t\]+_*init_iconv_desc" } } */
/* Contributed by Andrew Pinski 26 Oct 2004 <pinskia@gcc.gnu.org> */

struct cset_converter { int func; int cd; };
void abort(void); int puts(const char*);
int f(int i){return i;}
void g(void){puts("hi");}
struct conversion
{
  int pair;
  int func;
  int fake_cd;
};
static const struct conversion conversion_tab[] = {
  { 2, 2, 2 },
  { 3, 3, 3 }
};
static struct cset_converter
init_iconv_desc (int i)
{
  struct cset_converter ret;
  unsigned i1;
  char *pair;
  if (!i)
    {
      ret.func = 0;
      ret.cd = 0;
      return ret;
    }
  pair = __builtin_alloca(i*3);
  for (i1 = 0; i1 < (sizeof (conversion_tab) / sizeof ((conversion_tab)[0])); i1++)
    if (i == conversion_tab[i1].pair)
      {
	ret.func = conversion_tab[i1].func;
	ret.cd = conversion_tab[i1].fake_cd;
	return ret;
      }
    
  ret.func = 2;
  ret.cd = f(i);
  if (ret.cd == 4)
    {
      g();
      ret.func = 4;
    }
  return ret;
}
struct f
{
  struct cset_converter a;
  struct cset_converter b;
};

void ff(struct f *a)
{
  a->a = init_iconv_desc(0);
  /* Compiler was illegally optimizing this call into a tailcall (jmp).  Radar 3830232 */
  a->b = init_iconv_desc(1);
}

int main(void)
{
  struct f a;
  ff (&a);
  if (a.a.func!=0 || a.a.cd !=0)
    abort();
  if (a.b.func!=2 || a.b.cd !=1)
    abort();
  return 0;
}
