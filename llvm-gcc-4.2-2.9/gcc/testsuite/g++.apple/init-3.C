/* APPLE LOCAL file 6052773 */
/* { dg-do compile } */
/* { dg-options "" } */
struct pthread_once_t { };
struct test {
  pthread_once_t once;
};

int main(void) {
  struct test foo = { 
    once: PTHREAD_ONCE_INITIALIZER /* { dg-error "not declared in this scope" } */
  };

  return 0;
}
