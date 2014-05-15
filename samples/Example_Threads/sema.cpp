// Example of a simple blocking queue using pthreads and semaphores

#include <pthread.h>
#include <semaphore.h>
#include <sstream>
#include <deque>

// Simple thread safe blocking queue using semaphores
template <typename T> class BlockingQueue
{
private:
  std::deque<T> _elements; // contents of the queue
  pthread_mutex_t _dequeLock; // lock for the deque
  sem_t _cap; // semaphore representing remaining capacity
  sem_t _size; // semaphore representing occupied size

public:
  BlockingQueue(size_t maxElements) {
    // set up the sync primitives
    pthread_mutex_init(&_dequeLock, NULL);
    sem_init(&_cap, 0, maxElements); // _cap starts out with maxElements posts
    sem_init(&_size, 0, 0);
  }

  ~BlockingQueue() {
    sem_destroy(&_size);
    sem_destroy(&_cap);
    pthread_mutex_destroy(&_dequeLock);
  }

  T read() {
    // wait for a post to _size indicating availability
    sem_wait(&_size);
    pthread_mutex_lock(&_dequeLock); // exclusive access to deque
    T result = _elements.front(); // get the elment
    _elements.pop_front();
    std::stringstream s;
    s << "\tqueue size after read: " << _elements.size();
    puts(s.str().c_str());
    pthread_mutex_unlock(&_dequeLock);
    sem_post(&_cap); // indicate an increase in queue capacity
  
    return result;
  }

  void write(const T &elem) {
    // wait for a post to _cap indicating room to write
    sem_wait(&_cap);
    pthread_mutex_lock(&_dequeLock); // exclusive access to deque
    _elements.push_back(elem); // push the element on
    std::stringstream s;
    s << "\tqueue size after write: " << _elements.size();
    puts(s.str().c_str());
    pthread_mutex_unlock(&_dequeLock);
    sem_post(&_size); // indicate an increase in element availability
  }
};

// global queue holding 2 strings
static BlockingQueue<std::string> sQueue(2);

// # of reads or writes each thread performs
static const int gReadWriteCount = 5;

// all threads start at the same time by waiting on a barrier
static pthread_barrier_t sBarrier;

// a thread proc that writes to the queue
static void *writerThreadProc(void *arg) {
  pthread_barrier_wait(&sBarrier);

  intptr_t threadId = (intptr_t)arg;
  for(int i = 0; i < gReadWriteCount; i++) { // write to the queue!
    std::stringstream s;
    s << "hello from writer thread " << threadId << "!";
    sQueue.write(s.str());
  }
  return NULL;
}

// a thread proc that reads from the queue
static void *readerThreadProc(void *arg) {
  pthread_barrier_wait(&sBarrier);

  intptr_t threadId = (intptr_t)arg;
  for(int i = 0; i < gReadWriteCount; i++) { // read from the queue!
    std::stringstream s;
    s << "reader " << threadId << ": " << sQueue.read();
    puts(s.str().c_str());
  }
  return NULL;
}

int main() {
  static const int threadCount = 5; // # of readers, writers
  pthread_t readerThreads[threadCount];
  pthread_t writerThreads[threadCount];

  pthread_barrier_init(&sBarrier, NULL, threadCount * 2);

  // create readers
  for(int i = 0; i < threadCount; i++)
    pthread_create(readerThreads + i, NULL, &readerThreadProc, (void *)i);
  // create writers
  for(int i = 0; i < threadCount; i++)
    pthread_create(writerThreads + i, NULL, &writerThreadProc, (void *)i);
  // wait for them all to finish
  for(int i = 0; i < threadCount; i++) {
    pthread_join(readerThreads[i], NULL);
    pthread_join(writerThreads[i], NULL);
  }
  puts("done!");
  return 0;
}
