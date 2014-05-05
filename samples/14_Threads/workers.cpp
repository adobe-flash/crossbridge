// Illustration of worker to thread relationship

#include <pthread.h>
#include <string.h>
extern "C" {
#include <sys/thr.h> // BSD-like thread calls
}

#include <AS3/AS3.h>
#include <AS3/AVM2.h> // low level sync primitives

#include <sstream>

// declare an AS3 var -- will be worker-local (not thread-local!)
package_as3(
"public var someAS3Val:int = 0"
);

pthread_key_t sThrIdKey; // TLD for storing our BSD-like thread id

// this function returns a strdup-ed string instead of printing directly
// because it will be called by avm2_ui_thunk where C I/O is unsafe
static void *getInfoString(void *arg) {
  int someAS3Val;
  AS3_GetScalarFromVar(someAS3Val, someAS3Val);

  long id;
  thr_self(&id); // get our BSD-like thread id

  std::stringstream s;
  s << (char *)arg << "thread id (from thr_self): " << id <<
      " / thread id (from TLS): " << (int)pthread_getspecific(sThrIdKey) <<
      " / someAS3Val: " << someAS3Val;
  return strdup(s.str().c_str());
}

const static int sNumThreads = 5;
pthread_t sThreads[sNumThreads];
volatile long sThreadIds[sNumThreads];

static void *threadProc(void *arg) {
  char *str;
  long id;
  thr_self(&id); // get our BSD-like thread id

  AS3_CopyScalarToVar(someAS3Val, id); // set someAS3Val to thread id for this Worker
  pthread_setspecific(sThrIdKey, (void *)id); // also set the thread local var to thread id

  str = (char *)getInfoString((void *)"self: ");
  puts(str);
  free(str);

  int index = (int)arg;

  // put our thread id in the list
  sThreadIds[index] = id;
  // notify anyone waiting for our id
  avm2_wake((long *)sThreadIds + index);

  // impersonate the next thread in the list
  int indexToImpersonate = (index + 1) % sNumThreads;

  // if the thread id for the thread we want to impersonate isn't set up
  // then wait for it using a "self" sleep, allowing other threads to impersonate us
  while(!sThreadIds[indexToImpersonate])
    avm2_self_msleep((long *)sThreadIds + indexToImpersonate, 10);

  std::stringstream s;
  s << "thread id " << id << " impersonating thread id " << sThreadIds[indexToImpersonate] << ": ";

  // impersonate the other thread -- we're also open to impersonation while this runs!
  str = (char *)avm2_thr_impersonate(sThreadIds[indexToImpersonate], getInfoString, (void *)s.str().c_str());
  puts(str);
  free(str);

  // set the id to zero to indicate we're done with it
  sThreadIds[indexToImpersonate] = 0;
  // notify anyone waiting for it
  avm2_wake((long *)sThreadIds + indexToImpersonate);

  // wait for our peer to clear using a "self" sleep, allowing other threads
  // to impersonate us
  while(sThreadIds[index])
    avm2_self_msleep((long *)sThreadIds + index, 10);

  std::stringstream sui;
  sui << "thread id " << id << " thunking to ui Worker: ";

  // run getInfoString on the ui Worker as us
  str = (char *)avm2_ui_thunk(getInfoString, (void *)sui.str().c_str());
  puts(str);
  free(str);

  return NULL;
}

int main() {
  long id;
  thr_self(&id);
  AS3_CopyScalarToVar(someAS3Val, id); // set someAS3Val to thread id for this Worker

  pthread_key_create(&sThrIdKey, NULL); // set up a thread local var
  pthread_setspecific(sThrIdKey, (void *)id); // set it to thread id for us

  char *str;

  str = (char *)getInfoString((void *)"main thread: ");
  puts(str);
  free(str);
  str = (char *)avm2_ui_thunk(getInfoString, (void *)"main thread thunking to ui Worker: ");
  puts(str);
  free(str);

  for(int i = 0; i < sNumThreads; i++)
    pthread_create(sThreads + i, NULL, threadProc, (void *)i);
  for(int i = 0; i < sNumThreads; i++)
    pthread_join(sThreads[i], NULL);
  puts("done!");
  return 0;
}
