/* Radar 5636185  */
/* { dg-do compile { target *-*-darwin* } } */
/* LLVM LOCAL - -fverbose-asm
/* { dg-options "-O0 -gdwarf-2 -dA -fverbose-asm -c -Wno-deprecated-declarations" } */
/* { dg-final { scan-assembler "DW_AT_MIPS_linkage_name" } } */
/* { dg-final { scan-assembler "_Z7marker1v" } } */
/* { dg-final { scan-assembler "_Z7marker2v" } } */
/* { dg-final { scan-assembler "_Z6foobari" } } */
/* { dg-final { scan-assembler-not "_ZZ4mainEN10InnerLocal16NestedInnerLocal7nil_fooEi" } } */
/* { dg-final { scan-assembler-not "_ZZ4mainEN10InnerLocal6il_fooERKh" } } */

void marker1 (void)
{ 
}

void marker2 (void)
{
}
  
int foobar (int x)
{
  class Local {
  public:
    int loc1;
    char loc_foo (char c)
    {
      return c + 3;
    }
  };

  Local l;
  static Local l1;
  char  c;

  marker1 ();

  l.loc1 = 23;

  c = l.loc_foo('x');
  return c + 2;
}

int main()
{
  int c;
  
  c = foobar (31);
  
 { // inner block
   class InnerLocal {
   public:
     char ilc;
     int * ip;
     int il_foo (unsigned const char & uccr)
     {
       return uccr + 333;
     }
     class NestedInnerLocal {
     public:
       int nil;
       int nil_foo (int i)
       {
         return i * 27;
       }
     };
     NestedInnerLocal nest1;
   };

   InnerLocal il;

   il.ilc = 'b';
   il.ip = &c;
   marker2();
 }
}
