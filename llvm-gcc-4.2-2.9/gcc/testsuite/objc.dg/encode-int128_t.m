/* APPLE LOCAL file radar 5996271 */
/* encodeing support for 128-bit ints. */
/* { dg-do run } */

extern void abort(void);

int main () {
#if __LP64__
    if (@encode(__uint128_t)[0] != 'T')
      abort();
    if (@encode(__int128_t)[0] != 't')
      abort();
#endif
    return 0;
}

