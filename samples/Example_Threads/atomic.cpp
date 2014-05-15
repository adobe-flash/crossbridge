// Simple example of atomic operations

#include <pthread.h>
#include <iostream>
#include <sstream>
#include <time.h>

// atomically incremented thread counters
static volatile int sThreadCounter = 0;

struct Link {
  Link *next;
  std::string str;
};

typedef Link *LinkPtr;

static volatile LinkPtr sLinkedListHead = NULL;

// all threads start at the same time by waiting on a barrier
static pthread_barrier_t sBarrier;

// thread proc that adds links to the linked list
// represented by sLinkedListHead
static void *threadProc(void *arg) {
  // get a unique id by atomically incrementing the thread counter
  int selfAssignedId = __sync_fetch_and_add(&sThreadCounter, 1);

  pthread_barrier_wait(&sBarrier);

  for(int iter = 0; iter < 5; iter++) {
    std::stringstream s;
    s << "hello from thread " << selfAssignedId << " iter # " << iter << "\n";
    // set up a new link for the linked list
    LinkPtr newLink = new Link();
    newLink->str = s.str();
    // link it in without locking
    for(;;) {
      // fetch the current linked list head
      LinkPtr curHead = sLinkedListHead;
      // optimistically set it as the "next" in our link
      newLink->next = curHead;
      // if we can swap our link in without the head changing, we succeeded!
      if(__sync_bool_compare_and_swap(&sLinkedListHead, curHead, newLink))
        break;
    }
  }
  return NULL;
}

int main() {
  static const int threadCount = 10; // total # threads
  pthread_t threads[threadCount];
 
  pthread_barrier_init(&sBarrier, NULL, threadCount);

  for(int i = 0; i < threadCount; i++)
    pthread_create(threads + i, NULL, threadProc, NULL);
  for(int i = 0; i < threadCount; i++)
    pthread_join(threads[i], NULL);

  Link *cur = sLinkedListHead;
  while(cur) { // traverse the linked list!
    std::cout << cur->str; // print out the string
    Link *next = cur->next;
    delete cur; // delete link
    cur = next; // go to next!
  }
  puts("done!");
  return 0;
}
