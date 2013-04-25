/* APPLE LOCAL file radar 6237616  */
/* { dg-do compile } */
/* { dg-options "-g -O0 -dA -mmacosx-version-min=10.6" }*/
/* { dg-final { scan-assembler "line 15+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x0+\[ \t]+\[#;@]\[ \t]DW_LNE_set_address+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x\[0-9a-f]+\[ \t]+\[#;@]\[ \t]uleb128 0x\[0-9a-f]+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x2+\[ \t\n]+\[ \t]\[\\.long;\\.quad]+\[ \t]LM\[0-9]+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x\[0-9a-f]+\[ \t]+\[#;@]\[ \t]line 16"} } */
/* { dg-final { scan-assembler-not "line 15+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x0+\[ \t]+\[#;@]\[ \t]DW_LNE_set_address+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x\[0-9a-f]+\[ \t]+\[#;@]\[ \t]uleb128 0x\[0-9a-f]+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x2+\[ \t\n]+\[ \t]\[\\.long;\\.quad]+\[ \t]LM\[0-9]+\[ \t\n]+\[ \t]\\.byte+\[ \t]0x\[0-9a-f]+\[ \t]+\[#;@]\[ \t]DW_LNS_copy" } } */

#include <stdio.h>
#line 3
int 
main ()
{
  int val_1, val_2;

  int (^my_block) (int);

  if (1)
    {
      __block int use_by_ref = 1;
      int use_by_value = 0xfefefefe;
      
      my_block = ^ (int in_value) {
	use_by_ref++;
	printf ("Block function has: %d %d %d.\n", in_value, use_by_value, use_by_ref);
	return in_value + use_by_ref + use_by_value;
      };
      use_by_ref++;
      val_1 = my_block (10);
    }
      
  val_1 = my_block (20);
  val_2 = my_block (30);
  printf ("Got: %d and %d.\n", val_1, val_2);
  
  return 0;
}
