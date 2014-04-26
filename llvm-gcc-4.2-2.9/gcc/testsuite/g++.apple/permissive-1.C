/* APPLE LOCAL file pedwarn for a release 5493351 */
/* { dg-do compile } */
/* { dg-options { -O1 -fpermissive -w } } */
/* { dg-final { scan-assembler "bogus" } } */
/* Radar 5546638 */

extern int bogus;

int boogus_s;

void Binder()
{
  switch (boogus_s) {
  case 0x2:
    int jjj = 1;
    break;
  case 0xb:
    bogus=2;
    break;
  }
}
