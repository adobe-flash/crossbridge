#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>

void *f(void *a){
  static __thread intptr_t i = 1;
  i++;
  return (void *)i;
}

int main() {
  pthread_t t;
  intptr_t ret;
  pthread_create(&t, NULL, f, NULL);
  pthread_join(t, (void **) &ret);
  printf("Thread 1: %d\n", (int) ret);
  pthread_create(&t, NULL, f, NULL);
  pthread_join(t, (void **) &ret);
  printf("Thread 2: %d\n", (int) ret);
  return 0;
}
