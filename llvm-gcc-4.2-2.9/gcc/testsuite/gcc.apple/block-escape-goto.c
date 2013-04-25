/* APPLE LOCAL file radar 6083129 byref escapes */
/* { dg-options "-fblocks" } */
/* { dg-do compile } */

int main(int p) {
  p = -5;
  __block int O1;
  int i;
 LOUT: ;
  for (i = 1; i < 100; i++) {
    __block int I1;
    while (p < 0) {
      __block int II1;	
      if (p == 100)
	goto LOUT;
      ++p;
      if (p == 2345)
	break;
    }
  }
  return 0;
}
