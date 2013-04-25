/* APPLE LOCAL file unsigned wraps 6486153 */
/* { dg-do run } */

int main(int argc, const char **argv) {
    unsigned int foo = 0xfffffff0u + argc - 1;
    if (((11u * foo) % 11u) == 4)
      return 0;
    return 1;
}
