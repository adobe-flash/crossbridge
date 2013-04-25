/* APPLE LOCAL file 7105615 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-strict-overflow" } */
int
main() {
  unsigned char uch;
  unsigned int ui = 0;

  /* Original failure was an infinite loop: */
  for (uch = 0; uch < 0x80; uch++)
    ui = uch + ui;
  return ui != 8128;	/* Return 0 for success. */
}
