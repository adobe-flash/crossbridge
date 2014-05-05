// Example of a simple blocking queue using pthreads and conditions

#include <pthread.h>
#include <sstream>
#include <deque>

// Simple thread safe blocking queue using conditions
template <typename T> class BlockingQueue
{
private:
  // contents of the queue
  std::deque<T> _elements;
  // max # of elements allowed
  size_t _maxElements;
  // mutex used to synchronize access to the queue
  pthread_mutex_t _mutex;
  // conditions to signal queue reads and writes
  pthread_cond_t _readCond, _writeCond;

public:
  BlockingQueue(size_t maxElements) : _maxElements(maxElements) {
    // set up syncronization primitives
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init(&_readCond, NULL);
    pthread_cond_init(&_writeCond, NULL);
  }

  ~BlockingQueue() {
    // clean up
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_readCond);
    pthread_cond_destroy(&_writeCond);
  }

  T read() {
    pthread_mutex_lock(&_mutex); // lock the mutex
    while(!_elements.size()) { // empty? wait for a write
      puts("\tqueue empty: read blocked");
      pthread_cond_wait(&_writeCond, &_mutex);
    }
    T result = _elements.front(); // get the result
    _elements.pop_front(); // remove it from the deque
    pthread_cond_signal(&_readCond); // signal that we did a read
    pthread_mutex_unlock(&_mutex); // unlock the mutex
    return result;
  }

  void write(const T &elem) {
    pthread_mutex_lock(&_mutex); // lock the mutex
    while(_elements.size() >= _maxElements) { // full? wait for a read
      puts("\tqueue full: write blocked");
      pthread_cond_wait(&_readCond, &_mutex);
    }
    _elements.push_back(elem); // add the element to the deque
    pthread_cond_signal(&_writeCond); // signal that we did a write
    pthread_mutex_unlock(&_mutex); // unlock the mutex
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
