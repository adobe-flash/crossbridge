/* APPLE LOCAL file 4420068 */
/* Exercise zero-length fields. */

/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=gnu99" } */

#pragma reverse_bitfields on
#pragma ms_struct on
struct empty {};

struct uc0 { unsigned char content[0]; };

struct nest_uc0 { struct uc0 foo; };

struct uc0_uib2 { unsigned char foo[0]; unsigned int xx : 2; };

struct nest_nest_uc0_uib3 { struct nest_uc0 xx; unsigned int yy : 3; };

struct nest_empty_uib3 { struct empty e; unsigned int zz : 3; };

union size0 { struct empty e; int i[0]; };

union u0 { struct uc0 ss; unsigned int i; };

union u1 { struct nest_uc0 ss; unsigned int i; };

union u2 { struct uc0_uib2 ss; unsigned int i; };

union u3 { struct nest_nest_uc0_uib3 ss; unsigned int i; };

union u4 { struct nest_empty_uib3 ss; unsigned int i; };

union u5 { union size0 ss; unsigned int i; };

struct advance {
  char c;
  int spacer[0];
  char d;
};

struct advance2 {
  char c;
  struct empty s;
  char d;
};

struct advance3 {
  char c;
  union size0 u;
  char d;
};

int main() {

  if (sizeof(struct empty) !=0 ||
    sizeof(struct uc0) !=0
    || sizeof(struct uc0_uib2) != 4
    || sizeof(struct nest_nest_uc0_uib3) != 4
    || sizeof(struct nest_empty_uib3) != 4
    || sizeof(union size0) != 0
    || sizeof(union u0) != 4
    || sizeof(union u1) != 4
    || sizeof(union u2) != 4
    || sizeof(union u3) != 4
    || sizeof(union u4) != 4
    || sizeof(union u5) != 4
    || sizeof(struct advance) != 8
    || sizeof(struct advance2) != 2
    || sizeof(struct advance3) != 8)
    return 42;
  return 0;
}
