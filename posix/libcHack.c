// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/cdefs.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/fbio.h>
#include <sys/consio.h>
#include <sys/kbio.h>
#include <sys/termios.h>
#include <sys/sysctl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/thr.h>
#include <sys/umtx.h>
#include <math.h>
#include "libc_private.h"
#include "AS3/AS3.h"
#include "AS3/AVM2.h"

#define CONC_DBG 0
#define ARB static

extern void abort();
extern unsigned avm2_cmpSwapUns(unsigned *, unsigned, unsigned);
extern void avm2_mfence();

#define MIN_TID 8 // minimum viable bsd thread id

// have we created any threads? not exactly the same as __isthreaded (which is for pthreads)
static bool sIsThreaded = false;

// like printf, but uses trace()
int tprintf(const char *fmt, ...);
// enter and exit the arbitration lock
ARB void arb_enter();
ARB void arb_exit();

// CModule.serviceUIRequests() calls this
void (*flascc_uiTickProc)() = NULL;
int flascc_maxWorkers = 64; // undocumented but public max on # of WORKERS -- thread creation will fail if >= this many live WORKERS in domain

static int thrId();
static int thrRealId();
static void *thrTCB();

static unsigned *thrSelfLockTCB(void *tcbp)
{
  static unsigned sLock = -1U;
  // keep in sync w/ pthread_md.h and struct tcb
  void **tcb = (void **)tcbp;
  unsigned *result = (tcbp != (void *)-1) ? (unsigned *)(tcb + 4) : &sLock;
  if(sLock == -1U)
    sLock = thrRealId()+1;
  // handle transition to threaded
  if(sLock && result != &sLock)
  {
    *result = sLock;
    sLock = 0;
  }
  return result;
};

static volatile unsigned *thrSelfLockedCountTCB(void *tcbp)
{
  static volatile unsigned sCounter = 1;
  // keep in sync w/ pthread_md.h and struct tcb
  void **tcb = (void **)tcbp;
  volatile unsigned *result = (tcbp != (void *)-1) ? (unsigned *)(tcb + 5) : &sCounter;
  // handle transition to threaded
  if(sCounter && result != &sCounter)
  {
    *result = sCounter;
    sCounter = 0;
  }
  return result;
};

static unsigned *thrSelfLock()
{
  return thrSelfLockTCB(thrTCB());
}

static unsigned *thrSelfLockedCount()
{
  return thrSelfLockedCountTCB(thrTCB());
}

static void *uiThreadESPInit(int *psize) __attribute__((noinline));
static void *uiThreadESPInit(int *psize)
{
  void *result;
  int size;
  __asm __volatile__ (
    "%0 = 0; try { "
    "import flash.display.Sprite; "
    "if (Sprite) {"
    "%0 = workerClass.current.getSharedProperty('flascc.uiThread.esp_init'); "
    "%1 = workerClass.current.getSharedProperty('flascc.uiThread.stackSize'); "
    "} "
    "} catch(e:*) {}"
    : "=r"(result), "=r"(size));
  if(psize)
    *psize = size;
  return result;
}

static int uiThreadIdRaw(int def) __attribute__((noinline));
static int uiThreadIdRaw(int def)
{
  int result;
  __asm __volatile__ (
    "%0 = %1; try { "
      "import flash.display.Sprite; "
      "if (Sprite) "
      "{ "
        "var utid:* = workerClass.current.getSharedProperty('flascc.uiThread.threadId'); "
        "%0 = (typeof(utid) == 'undefined') ? %1 : utid; "
      "} "
    "} catch(e:*) {}"
    : "=r"(result) : "r"(def));
  return result;
}

static int uiThreadId()
{
  return uiThreadIdRaw(-1);
}

int avm2_is_ui_worker()
{
  static int sUITID = -2;

  if(sUITID == -2)
  {
    sUITID = uiThreadId();
    if(sUITID >= 0)
      sIsThreaded = true;
  }
  return (sUITID < 0) || (thrRealId() == sUITID);
}

// all mutexes here for convenience (acquire in this order if acquiring multiple)
static unsigned sThreadListMtx = 0;
static unsigned skpmallocMtx = 0;
static unsigned smmapMtx = 0;

FILE *_nsyyin;
int _nsyylineno;
char *_nsyytext;
int nsdispatch(void *a1, const int a2[], const char *a3,  const char *a4, const int a5[], ...) { abort(); return -1; }
void _nsyyerror(const char *a1) { abort(); }
int _nsyylex() { abort(); return -1; }
void _nsdbtdump(const void *a1) { abort(); }

int _getlogin(char *namebuf, u_int namelen) { strcpy(namebuf, "alchemist"); return 0; }

int getfsstat(struct statfs *a1, long a2, int a3)
{
    return 0; // should probably be 1 for "/"
}

void outb(int x, char y)
{ 
}

char inb(int x)
{
    return '\0';
}

int select(int nd, fd_set *in, fd_set *ou, fd_set *ex, struct timeval *tv)
{
    return -1;
}

int mprotect(const void *addr, size_t len, int prot)
{
    return -1;   
}

// printf to gdb's stderr via gdb write syscall
int gdb_printf(const char *fmt, ...)
{
    char cmd[256];
    char buf[16384];
    va_list ap;
    int result;

    va_start(ap, fmt);

    // construct the string in "buf"
    result = vsnprintf(buf, sizeof(buf), fmt, ap);
    buf[sizeof(buf)-1] = 0;
    // construct the write syscall for gdb
    sprintf(cmd, "Fwrite,2,%x,%x", (unsigned)buf, strlen(buf));
    inline_as3("import com.adobe.flascc.AlcDbgHelper");
    // send the command to gdb (expecting "Fxx" result)
    inline_as3("AlcDbgHelper.gdb(CModule.readString(%0, %1), /^F/)" : : "r"(cmd), "r"(strlen(cmd)));
    va_end(ap);
    return result;
}

#define INT_BITS (sizeof(int)*8)
#define PAGE_SIZE 4096
#define PAGE_SIZE1 (PAGE_SIZE-1)

// return non-zero if bit at index is set, zero if cleared
static int bsGet(int *bits, int index)
{
    return bits[index / INT_BITS] & (1 << (index % INT_BITS));
}

// set bit at index
static void bsSet(int *bits, int index)
{
    bits[index / INT_BITS] |= (1 << (index % INT_BITS));
}

// set a contiguous range of bits starting at index of length num
static void bsContigSet(int *bits, int index, int num)
{
    while(num--) {
        bsSet(bits, index++);
    }
}

// clear bit at index
static void bsClear(int *bits, int index)
{
    bits[index / INT_BITS] &= ~(1 << (index % INT_BITS));
}

// clear a contiguous range of bits starting at index of length num
static void bsContigClear(int *bits, int index, int num)
{
    while(num--) {
        bsClear(bits, index++);
    }
}

// find num contiguous clear bits searching backwards along [0, end)
// return -1 if not found, index of first clear bit on success
static int bsRFindContigCleared(int *bits, int end, int num)
{
    int curBit = end;

top:
    while(curBit >= num) {
        int s;

        for(s = curBit-num; s < curBit; s++) {
            if(bsGet(bits, s)) {
                curBit = s;
                goto top;
            }
        }
        return curBit-num;
    }
    return -1;
}

// tries to find num contig clear bits using bsRFindContigCleared and,
// if found, sets them all, and returns the index of the first bit
// if not found, returns -1
static int bsAlloc(int *bits, int size, int num)
{
    int alloced = bsRFindContigCleared(bits, size, num);

    if(alloced >= 0) {
        bsContigSet(bits, alloced, num);
    }
    return alloced;
}

// bitset indicating which PAGE_SIZE-sized blocks are in use
static int mmapBlockAllocBitSet[32768]; // 2^15 words addresses 4Gb of pages
// number of PAGE_SIZE-sized blocks mmap is tracking via mmapBlockAllocBitSet
static int mmapBlockCount = 0;

// mark implicitly alloced pages as alloced / freed as freed
static void handleNonMMapAllocs(int size)
{
    int blockCount;

    if(size < 0)
      inline_as3("%0 = (ram.length + %1) / %2" : "=r"(blockCount) : "r"(PAGE_SIZE1), "r"(PAGE_SIZE));
    else
      blockCount = (size + PAGE_SIZE1) / PAGE_SIZE;
    if(mmapBlockCount < blockCount) {
        bsContigSet(mmapBlockAllocBitSet, mmapBlockCount, blockCount - mmapBlockCount);
    }
    mmapBlockCount = blockCount;
}

#define MMAP_LOG 0

int _kevent(int fd, void *changelist, int nchanges, void *eventlist, int nevents, void *timeout)
{
abort();
return -1;
}

int kqueue()
{
abort();
return -1;
}

int setpriority(int which, int who, int prio)
{
    return 0;
}

ssize_t __sys_pread(int fd, void *iovp, u_int iovcnt, off_t offset)
{
abort();
return -1;
}

ssize_t __sys_pwrite(int fd, const void *iovp, u_int iovcnt, off_t offset)
{
abort();
return -1;
}

ssize_t __sys_freebsd6_pread(int fd, void *iovp, size_t arg, int iovcnt, off_t offset)
{
abort();
return -1;
}

ssize_t __sys_freebsd6_pwrite(int fd, const void *iovp, size_t arg, int iovcnt, off_t offset)
{
abort();
return -1;
}

int _sendto(int s, const void *buf, size_t len, int flags, void *to, unsigned int tolen)
{
abort();
return -1;
}
				    					
int _sendmsg(int s, void *msg, int flags)
{
abort();
return -1;
}

int _connect(int s, void *name,  int namelen)
{
abort();
return -1;
}

int _execve(char *fname, char **argv, char **envv)
{
errno = EACCES;
return -1;
}

int getrlimit()
{
  abort();
  return -1;
}

int setrlimit()
{
  abort();
  return -1;
}

int __sys_ftruncate(int fd, off_t length)
{
    abort();
    return -1;
}

int __sys_freebsd6_ftruncate(int fd, int arg, off_t length)
{
    abort();
    return -1;
}

int __sys_truncate(const char *path, off_t length)
{
    abort();
    return -1;
}

int __sys_freebsd6_truncate(const char *path, int arg, off_t length)
{
    abort();
    return -1;
}

int avm2_haveWorkers()
{
  static int avm2_haveWorkers = -1;
  if(avm2_haveWorkers == -1)
    __asm("%0 = Boolean(workerClass)" : "=r"(avm2_haveWorkers));
  return avm2_haveWorkers;
}

static size_t __attribute__ ((noinline)) avm2_casRamLength(size_t cur, size_t size)
{
  size_t result;
  if(avm2_haveWorkers())
    __asm __volatile__ ("try { %0 = ram.atomicCompareAndSwapLength(%1, %2) } catch(e:*) { if(C_Run.throwWhenOutOfMemory) throw e; %0 = -1; }" : "=r"(result) : "r"(cur), "r"(size));
  else
  {
    __asm __volatile__(
      "%0 = ram.length\n"
      "if(%0 == %1) { try { ram.length = %2 } catch(e:*) { if(C_Run.throwWhenOutOfMemory) throw e; %0 = -1; } }"
      : "+r"(result) : "r"(cur), "r"(size)
    );
  }
  return result;
}

static void mmap_log(const char* str, int val)
{
    #if MMAP_LOG
        tprintf("%s%d\n", str, val);
        //inline_as3("trace('" str "' + %0)" : : "r"(val));
    #endif
}

extern void *mmap_file(void *, size_t, int, int, int, off_t);

void* __attribute__ ((noinline)) __sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t o2)
{
    void *result = 0;
    int curLen;

    if(fd >= 0)
      return mmap_file(addr, len, prot, flags, fd, o2);

    if(!(flags == MAP_FILE || flags & MAP_ANON)) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    if(prot & PROT_EXEC) {
        errno = EINVAL;
        return MAP_FAILED;
    }

#if MMAP_LOG
    tprintf("mmap(%p, %d, %d, %d, %d, %d)\n", addr, len, prot, flags, fd, o2);
#endif
    mmap_log("mmap len: ", len);
    mmap_log("mmap before handleNonMMapAllocs: mmapBlockCount = ", mmapBlockCount);
    if(avm2_haveWorkers()) avm2_lock(&smmapMtx);
tryAlloc: // we need to spin trying to grow memory length in case someone other than
          // mmap (like the thread arbitration, or hand-written as3) is changing it!
    AS3_GetScalarFromVar(curLen, ram.length);
    handleNonMMapAllocs(curLen);
    mmap_log("mmap after handleNonMMapAllocs: mmapBlockCount = ", mmapBlockCount);

    // alloc them!
    {
        int blocksNeeded = (len + PAGE_SIZE1) / PAGE_SIZE;
        int blockAlloc = bsAlloc(mmapBlockAllocBitSet, mmapBlockCount, blocksNeeded);

        mmap_log("mmap blocksNeeded: ", blocksNeeded);
        mmap_log("mmap blockAlloc: ", blockAlloc);
        if(blockAlloc >= 0) { // already got 'em -- reuse
            result = (void *)(blockAlloc * PAGE_SIZE);
        } else {
            int newBlockCount = mmapBlockCount + blocksNeeded;
            result = (void *)(mmapBlockCount * PAGE_SIZE); // need to grow
            int newlen = avm2_casRamLength(curLen, newBlockCount * PAGE_SIZE);
            if(newlen == -1) {
              #if MMAP_LOG
              tprintf("mmap failed with OOM\n");
              #endif
              if(avm2_haveWorkers()) avm2_unlock(&smmapMtx);
              errno = ENOMEM;
              return MAP_FAILED;
            }
            #if MMAP_LOG
              tprintf("mmap avm2_casRamLength: %d\n", newlen);
            #endif

            if(newlen != curLen) {
              #if MMAP_LOG
              tprintf("mmap cas fail: %d %d\n", newlen, curLen);
              #endif
              goto tryAlloc; // try again if len changed in the interim!
            }
            bsContigSet(mmapBlockAllocBitSet, mmapBlockCount, blocksNeeded);
            mmapBlockCount = newBlockCount;
            #if MMAP_LOG
                int rlen;
                AS3_GetScalarFromVar(rlen, ram.length);
                mmap_log("mmap ram.length: ", rlen);
            #endif
        }
    }

    if(avm2_haveWorkers()) avm2_unlock(&smmapMtx);

    //mmap_log("mmap: returns: " , result);

    memset(result, 0, len);

    return result;
}

void* __attribute__ ((noinline)) __sys_freebsd6_mmap(void *addr, size_t len, int prot, int flags, int fd, int o1, off_t o2)
{
    return __sys_mmap(addr, len, prot, flags, fd, o2);
}

int __attribute__ ((noinline)) munmap(void *addr, size_t len)
{
    int blockStart = (int)addr / PAGE_SIZE;
    int blockEnd = ((int)addr + len + PAGE_SIZE1) / PAGE_SIZE;

    if(avm2_haveWorkers()) avm2_lock(&smmapMtx);
    bsContigClear(mmapBlockAllocBitSet, blockStart, blockEnd - blockStart);

    mmap_log("munmap addr: ", addr);
    mmap_log("munmap len: ", len);
    mmap_log("munmap blockStart: ", blockStart);
    mmap_log("munmap blockEnd: ", blockEnd);
    handleNonMMapAllocs(-1);
    // reclaim memory >= 1M
    {
        int blockCount = mmapBlockCount;

        while(blockCount && !bsGet(mmapBlockAllocBitSet, blockCount-1)) {
            blockCount--;
        }
        if(mmapBlockCount - blockCount >= 0x100) {
            int curLen = mmapBlockCount * PAGE_SIZE;
            mmap_log("munmap reclaiming to: ", blockCount);
            int newlen = avm2_casRamLength(curLen, blockCount * PAGE_SIZE);
            if(newlen == -1) {
              if(avm2_haveWorkers()) avm2_unlock(&smmapMtx);
              errno = EINVAL;
              return -1;
            }

            if(newlen == curLen) // optimistically shrink
              mmapBlockCount = blockCount;
#if MMAP_LOG
            int rlen;
            AS3_GetScalarFromVar(rlen, ram.length);
            mmap_log("mmap ram.length: ", rlen);
#endif
        }
    }
    if(avm2_haveWorkers()) avm2_unlock(&smmapMtx);
    return 0;
}

int madvise(void *addr, size_t len, int advice)
{
    return 0;
}

int gettimeofday(struct timeval *tp,
                 struct timezone *tzp) __attribute__((noinline));
int gettimeofday(struct timeval *tp,
                 struct timezone *tzp)
{
// TODO: fixme, this sucks
    inline_nonreentrant_as3(
        "var d = (new Date);\n"
        "%0 = d.time / 1000\n"
        "%1 = d.getMilliseconds() * 1000\n"
        : "=r"(tp->tv_sec), "=r"(tp->tv_usec) : );

    return 0;
}

int clock_gettime(clockid_t clk_id,
                  struct timespec *tp) __attribute__((noinline));
int clock_gettime(clockid_t clk_id,
                  struct timespec *tp)
{
    // TODO: fixme, this is totally not cool
    inline_nonreentrant_as3 (
                      "var d = (new Date);\n"
                      "%0 = d.time / 1000\n"
                      "%1 = d.getMilliseconds() * 1000000\n"
                      : "=r"(tp->tv_sec), "=r"(tp->tv_nsec) : );
    
    return 0;
}

double pow(double x, double y)
{
    double ret;
    inline_nonreentrant_as3(
        "%0 = Math.pow(%1, %2);\n"
        : "=r"(ret) : "r"(x), "r"(y));
    return ret;
}

double sin(double x)
{
    double ret;
    inline_nonreentrant_as3(
        "%0 = Math.sin(%1);\n"
        : "=r"(ret) : "r"(x));
    return ret;
}

double cos(double x)
{
    double ret;
    inline_nonreentrant_as3(
        "%0 = Math.cos(%1);\n"
        : "=r"(ret) : "r"(x));
    return ret;
}

double sqrt(double x)
{
    double ret;
    inline_nonreentrant_as3(
        "%0 = Math.sqrt(%1);\n"
        : "=r"(ret) : "r"(x));
    return ret;
}

double fabs(double x)
{
    if (x < 0) {
        x = -x;
    }
    return(x);
}

int profil(char *samples, size_t size, vm_offset_t offset, int scale)
{
    return 0;
}

int getrusage()
{
    return -1;
}

void *sbrk(int size)
{
    void *result;
    if(size == 0)
        inline_nonreentrant_as3("%0 = ram.length;\n" : "=r"(result));
    else
        inline_nonreentrant_as3("%0 = sbrk(%1, %2);\n" : "=r"(result) : "r"((size + PAGE_SIZE1) & ~PAGE_SIZE1), "r"(PAGE_SIZE));

    if(result == -1) {
    #if MMAP_LOG
      mmap_log("sbrk failed, %d\n", result);
    #endif
      errno = ENOMEM;
    }
    
    return result;
}

int _select(int x1, fd_set *x2, fd_set *x3, fd_set *x4, struct timeval *x5)
{
    return -1;
}

void _set_tp(void *tp)
{
    abort();
}

int utimes(const char * a1, const struct timeval * a2)
{
  return -1;
}
int setitimer(int a0, const struct itimerval *a1, struct itimerval *a2)
{
  abort();
  return -1;
}

int utrace(char *msg, int x)
{
    return 0;
}

int __sys_sigaction(int x, const void* y, void* z)
{
    return 0;
}

int sigaction(int x, const void *y, void *z) __attribute__((weak, alias("__sys_sigaction")));
int _sigaction(int x, const void *y, void *z) __attribute__((weak, alias("__sys_sigaction")));

static char __avm2_ostype[] = "FreeBSD";
static char __avm2_osrelease[] = "8.1-RELEASE";
static char __avm2_version[] = "FreeBSD 8.1-RELEASE";
static char __avm2_hostname[] = "flascc.example.com";
static char __avm2_machine[] = "avm2";

// TODO is oldlenp allowed to be null?
// TODO ignoring newp, newlen?
int __sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp,
    void *newp, size_t newlen)
{
    void *p;
    int n;

    if (name[0] == CTL_KERN) {
        switch (name[1]) {
        case KERN_OSTYPE:
            *oldlenp = sizeof(__avm2_ostype);
            strcpy(oldp, __avm2_ostype);
            return 0;
        case KERN_OSRELEASE:
            *oldlenp = sizeof(__avm2_osrelease);
            strcpy(oldp, __avm2_osrelease);
            return 0;
        case KERN_VERSION:
            *oldlenp = sizeof(__avm2_version);
            strcpy(oldp, __avm2_version);
            return 0;
        case KERN_HOSTNAME:
            *oldlenp = sizeof(__avm2_hostname);
            strcpy(oldp, __avm2_hostname);
            return 0;
        case KERN_OSRELDATE:
            *oldlenp = sizeof(int);
            *((int*)oldp) = 800028;
            return 0;
        case KERN_USRSTACK:
            *oldlenp = sizeof(void *);
            __asm("%0 = ESP_init;" : "=r"(p));
            *((void**)oldp) = p;
            return 0;
        }
    } else if (name[0] == CTL_HW) {
        switch (name[1]) {
        case HW_MACHINE:
            *oldlenp = sizeof(__avm2_machine);
            strcpy(oldp, __avm2_machine);
            return 0;
	case HW_PAGESIZE:
	    *oldlenp = sizeof(int);
            __asm("%0 = pageSize;" : "=r"(n));
	    *(int *)oldp = n;
	    return 0;
	case HW_NCPU:
            *oldlenp = sizeof(int);
            *(int *)oldp = 8; //TODO
            return 0;
        }
    }

    return -1;
}

int _fstatfs(int fd, void *buf)
{
    return -1;
}

size_t readlink(const char* x, char* y, size_t z)
{
    return -1;
}

int issetugid()
{
    return 0;
}

package_as3("public const clockStart:Number = (new Date).time;");

clock_t clock()
{
    int result;

    inline_nonreentrant_as3(
        "%0 = (((new Date).time - clockStart) * %1) / 1000;"
        : "=r"(result) : "r"(CLOCKS_PER_SEC)
    );
    return result;
}

int setjmp(jmp_buf jb) __attribute__((noinline));
int setjmp(jmp_buf jb)
{
    int result;

    // using ebp in case someone wants to add a local var or something...
    // ebp should remain caller's esp even if esp is adjusted in this func
    inline_as3("%0 = setjmp(%1, sjid, ebp)" : "=r"(result) : "r"(jb));
    return result;
}


void longjmp(jmp_buf jb, int retval)
{
    inline_as3("longjmp(%0, %1)\n" : : "r"(jb), "r"(retval));
    //__builtin_unreachable();
}

void _fini()
{
    inline_as3("CModule.runDtors()");
}

static void uiThreadInit();

extern void flascc_pthreadsInit();

void _init()
{
    flascc_pthreadsInit();
    uiThreadInit(); // run this before ctors so ctors don't end up using uiTick w/ mini-stack
    // from within CModule.startBackground
    inline_as3("CModule.runCtors()");
}

//TODO: replace me with something efficient
double ldexp(double x, int n)
{
    double result = 0;
    inline_nonreentrant_as3("%0 = %1 * Math.pow(2, %2)\n" : "=r"(result) : "r"(x), "r"(n) );
    return result;
}

// causes the linker to include these...
// they still need to be in public-api.txt
void *__hack_used[] = {
    0
    ,&memcpy
    ,&memmove
    ,&ioctl
    ,&mmap_file
    ,&sigaction
    ,&_sigaction
    ,&ldexp
};

// is this gprof stuff?!
int etext;
int eprol;

static void *delegateExitToActiveConsoleThunk(void *code) __attribute__((noinline));
static void *delegateExitToActiveConsoleThunk(void *code)
{
  int result;
  __asm __volatile__("try { %0 = CModule.activeConsole.exit(%1); } catch(e:*)  { %0 = 0; }" : "=r"(result) : "r"((int)code));
  return (void *)result;
}

void _exit(int code)
{
  if(!avm2_ui_thunk(delegateExitToActiveConsoleThunk, (void *)code)) // try to delegate to the console on the ui thread
    inline_nonreentrant_as3("throw new Exit(%0)" : : "r"(code));
    //__builtin_unreachable();
}

unsigned int __sync_val_compare_and_swap_4(unsigned int *ptr, unsigned int oldval, unsigned int newval)
{
  unsigned int result = avm2_cmpSwapUns(ptr, oldval, newval);
#if CONC_DBG > 2
  long id;
  thr_self(&id);
  tprintf("(%ld) __sync_val_compare_and_swap_4(%p, %d, %d) => %d (%d)\n", id, ptr, oldval, newval, *ptr);
#endif
  return result;
}

// helper that emulates a masked cas
static inline unsigned int cmpSwapUnsMask(unsigned int *ptr, unsigned int oldval, unsigned int newval, unsigned int mask)
{
  unsigned int result;

  for(;;)
  {
    unsigned int moldval = *(volatile unsigned int *)ptr & ~mask;

    result = __sync_val_compare_and_swap_4(ptr, moldval | oldval, moldval | newval);

    if(result & ~mask == moldval) // didn't fail due to irrel bits?
      break;
    // failed due to irrel bits, spin...
  }
  return result & mask;
}

// TODO: CAS on non 32bit values is broken right now, we should disable it properly in the frontend at some point
unsigned short __sync_val_compare_and_swap_2(unsigned short *ptr, unsigned short oldval, unsigned short newval)
{
  inline_as3("trace((new Error()).getStackTrace() + \"\\n\")");
  inline_as3("trace(\"FlasCC does not support CAS on a non-32bit values..\")");
  abort();
  return 0;
  ///int shift = ((int)ptr & 2) * 8;
  //return cmpSwapUnsMask((unsigned int *)((int)ptr & ~2), (unsigned int)oldval << shift, (unsigned int)newval << shift, 0xffff << shift) >> shift;
}

unsigned char __sync_val_compare_and_swap_1(unsigned char *ptr, unsigned char oldval, unsigned char newval)
{
  inline_as3("trace((new Error()).getStackTrace() + \"\\n\")");
  inline_as3("trace(\"FlasCC does not support CAS on a non-32bit values..\")");
  abort();
  return 0;
  //int shift = ((int)ptr & 3) * 8;
  //return cmpSwapUnsMask((unsigned int *)((int)ptr & ~3), (unsigned int)oldval << shift, (unsigned int)newval << shift, 0xff << shift) >> shift;
}

bool __sync_bool_compare_and_swap_4(unsigned int *ptr, unsigned int oldval, unsigned int newval)
{
  return __sync_val_compare_and_swap_4(ptr, oldval, newval) == oldval;
}

bool __sync_bool_compare_and_swap(unsigned int *ptr, unsigned int oldval, unsigned int newval)
{
  return __sync_bool_compare_and_swap_4(ptr, oldval, newval);
}

unsigned int __sync_val_compare_and_swap(unsigned int *ptr, unsigned int oldval, unsigned int newval)
{
  return __sync_val_compare_and_swap_4(ptr,oldval,newval);
}


// TODO: CAS on non 32bit values is broken right now, we should disable it properly in the frontend at some point
bool __sync_bool_compare_and_swap_2(unsigned short *ptr, unsigned short oldval, unsigned short newval)
{
  inline_as3("trace((new Error()).getStackTrace() + \"\\n\")");
  inline_as3("trace(\"FlasCC does not support CAS on a non-32bit values..\")");
  abort();
  return false;
  //return __sync_val_compare_and_swap_2(ptr, oldval, newval) == oldval;
}

bool __sync_bool_compare_and_swap_1(unsigned char *ptr, unsigned char oldval, unsigned char newval)
{
  inline_as3("trace((new Error()).getStackTrace() + \"\\n\")");
  inline_as3("trace(\"FlasCC does not support CAS on a non-32bit values..\")");
  abort();
  return false;
  //return __sync_val_compare_and_swap_1(ptr, oldval, newval) == oldval;
}

static void lfence()
{
  avm2_mfence();
}

static void sfence()
{
  avm2_mfence();
}

static void mfence()
{
  avm2_mfence();
}

void __sync_synchronize()
{
  mfence();
}

/* TODO -- better way to do this? LLVM generates a symbol to "__sync_synchronize" directly, bypassing "libcalls" (where we add an underscore to these
kind of calls... so, add another with one less underscore...
*/
void _sync_synchronize()
{
  mfence();
}

unsigned int __sync_lock_test_and_set_4(unsigned int *ptr, unsigned int newval)
{
  unsigned int oldval;
  for(;;)
  {
    oldval = *(volatile unsigned int *)ptr;
    if(__sync_val_compare_and_swap_4(ptr, oldval, newval) == oldval)
      break;
  }
  return oldval;
}

unsigned short __sync_lock_test_and_set_2(unsigned short *ptr, unsigned short newval)
{
  unsigned short oldval;
  for(;;)
  {
    oldval = *(volatile unsigned short *)ptr;
    if(__sync_val_compare_and_swap_2(ptr, oldval, newval) == oldval)
      break;
  }
  return oldval;
}

unsigned char __sync_lock_test_and_set_1(unsigned char *ptr, unsigned char newval)
{
  unsigned char oldval;
  for(;;)
  {
    oldval = *(volatile unsigned char *)ptr;
    if(__sync_val_compare_and_swap_1(ptr, oldval, newval) == oldval)
      break;
  }
  return oldval;
}

void __sync_lock_release_4(unsigned int *ptr)
{
  *(volatile unsigned int *)ptr = 0;
  sfence();
}

void __sync_lock_release_2(unsigned short *ptr)
{
  *(volatile unsigned short *)ptr = 0;
  sfence();
}

void __sync_lock_release_1(unsigned char *ptr)
{
  *(volatile unsigned char *)ptr = 0;
  sfence();
}

#define GEN_SYNC_1(W, T, N, OP) \
T __sync_##N##_##W(T *ptr, T n) \
{ \
  T result; \
  T cur = *(volatile T *)ptr; \
  for(;;) \
  { \
    T val, cur1; \
    OP; \
    if(cur == (cur1 = __sync_val_compare_and_swap_##W(ptr, cur, val))) \
      break; \
    cur = cur1; \
  } \
  return result; \
}

#define GEN_SYNC_0(T, N, OP) \
T __sync_##N(T *ptr, T n) \
{ \
  T result; \
  T cur = *(volatile T *)ptr; \
  for(;;) \
  { \
    T val, cur1; \
    OP; \
    if(cur == (cur1 = __sync_val_compare_and_swap(ptr, cur, val))) \
      break; \
    cur = cur1; \
  } \
  return result; \
}

#define GEN_SYNC(N, OP) \
  GEN_SYNC_1(1, unsigned char, N, OP) \
  GEN_SYNC_1(2, unsigned short, N, OP) \
  GEN_SYNC_1(4, unsigned int , N, OP) \
  GEN_SYNC_0(unsigned int , N, OP)

GEN_SYNC(fetch_and_add, val = (result = cur) + n)
GEN_SYNC(fetch_and_sub, val = (result = cur) - n)
GEN_SYNC(fetch_and_or, val = (result = cur) | n)
GEN_SYNC(fetch_and_xor, val = (result = cur) ^ n)
GEN_SYNC(fetch_and_and, val = (result = cur) & n)
GEN_SYNC(fetch_and_nand, val = ~(result = cur) & n)

GEN_SYNC(add_and_fetch, result = val = cur + n)
GEN_SYNC(sub_and_fetch, result = val = cur - n)
GEN_SYNC(or_and_fetch, result = val = cur | n)
GEN_SYNC(xor_and_fetch, result = val = cur ^ n)
GEN_SYNC(and_and_fetch, result = val = cur & n)
GEN_SYNC(nand_and_fetch, result = val = ~cur & n)

// BSD atomics
/*
TODO do we need the complete set? this is the minimal set used by libthread
U	_atomic_add_barr_int
U	_atomic_add_int
U	_atomic_cmpset_int
U	_atomic_fetchadd_int
U	_atomic_store_rel_int
*/

int atomic_cmpset_int(volatile unsigned *dst, unsigned exp, unsigned src)
{
  return __sync_val_compare_and_swap_4((unsigned *)dst, exp, src) == exp;
}

unsigned atomic_fetchadd_int(volatile unsigned *p, unsigned v)
{
  return __sync_fetch_and_add((unsigned *)p, v);
}

void atomic_add_barr_int(volatile unsigned *p, unsigned v)
{
  __sync_fetch_and_add((unsigned *)p, v);
}

void atomic_add_int(volatile unsigned *p, unsigned v)
{
  __sync_fetch_and_add((unsigned *)p, v);
}

void atomic_store_rel_int(volatile unsigned *p, unsigned v)
{
//TODO is this right?
  lfence();
  *p = v;
}

typedef struct _s_ThreadEntry
{
  struct _s_ThreadEntry *next;
  void *tcb;
  int id;
} ThreadEntry;

// list of all known alive or zombie "bsd" threads
static ThreadEntry *sThreadListHead;

static void *thrTCB()
{
  void *result;

  __asm __volatile__ ("%0 = tcbp" : "=r"(result));
  return result;
}

static int thrId()
{
  int result;

  __asm __volatile__ ("%0 = threadId" : "=r"(result)); // volatile because of impersonation
  return result;
}

static int thrRealId()
{
  int result;

  __asm ("%0 = realThreadId" : "=r"(result));
  return result;
}

static void *thrImpersonate(int id, void *tcb, void *(*proc)(void *), void *arg, int *except) __attribute__((noinline));
static void *thrImpersonate(int id, void *tcb, void *(*proc)(void *), void *arg, int *except)
{
  void *result;
  __asm __volatile__ (
    "var oldId:int = threadId;"
    "var oldTcbp:int = tcbp;"
//"trace('thrImpersonate: ' + %2);"
    "var unlock:int = %6;"
    "var except:int = %7;"
    "try {"
      "threadId = %1;"
      "tcbp = %2;"
      "if(except) CModule.write32(except, 1);"
      "CModule.callI(%5, new <int>[]);"
      "%0 = CModule.callI(%3, new <int>[%4]);"
      "if(except) CModule.write32(except, 0);"
    "} catch(e:*) {"
      "if(!except) throw e;" // eat it if except != NULL
    "} finally {"
      "try { CModule.callI(unlock, new <int>[]); } catch(e:*) {}"
      "threadId = oldId;"
      "tcbp = oldTcbp;"
    "}" : "=r"(result) : "r"(id), "r"(tcb), "r"(proc), "r"(arg), "r"(avm2_self_lock), "r"(avm2_self_unlock), "r"(except));
  return result;
}

// requires the thread list lock to be held!
static ThreadEntry *thrEntryFromIdLocked(int id)
{
  ThreadEntry *entry;

  entry = sThreadListHead;
  while(entry && entry->id != id)
    entry = entry->next;
  return entry;
}

// ensure there's a thread entry for us!
static void ensureSelfThrEntry()
{
  ThreadEntry *entry;
  long tid;

  thr_self(&tid);
  avm2_lock(&sThreadListMtx);
  entry = thrEntryFromIdLocked(tid-MIN_TID);
  avm2_unlock(&sThreadListMtx);
  if(!entry)
  {
    entry = malloc(sizeof(ThreadEntry));
    entry->id = tid-MIN_TID;
    entry->tcb = thrTCB();
    avm2_lock(&sThreadListMtx);
    entry->next = sThreadListHead;
    sThreadListHead = entry;
    avm2_unlock(&sThreadListMtx);
  }
}

void *avm2_thr_impersonate(long tid, void *(*proc)(void *), void *arg)
{
  void *result;
  ThreadEntry *entry;

  ensureSelfThrEntry();
  avm2_lock(&sThreadListMtx);
  entry = thrEntryFromIdLocked(tid-MIN_TID);

//tprintf("avm2_thr_impersonate %ld %p\n", tid, entry);
  if(entry)
  {
    void *tcb = entry->tcb;
    unsigned selfLockCount;
    unsigned *selfLock = thrSelfLock();
    unsigned selfLockId = thrRealId()+1;

    avm2_unlock(&sThreadListMtx);
    if(selfLockId != avm2_locked_id(selfLock))
      selfLockId = 0;
    else
    {
      selfLockCount = *thrSelfLockedCount();
      *thrSelfLockedCount() = 0;
      avm2_unlock(selfLock);
    }
    result = thrImpersonate(tid-MIN_TID, tcb, proc, arg, NULL);
    if(selfLockId)
    {
      avm2_lock_id(thrSelfLock(), selfLockId); // lock might have changed if thread created!
      *thrSelfLockedCount() = selfLockCount;
    }

    return result;
  }
  avm2_unlock(&sThreadListMtx);
  return NULL;
}

int thr_create(ucontext_t *ctx, long *id, int flags)
{
  errno = ENOSYS;
  return -1;
}

/* stub that sets up tls stuff and then runs the given fun */
static void thread_run(ThreadEntry *entry, void (*start_func)(void *), void *start_func_arg)
{
  __asm __volatile__ ("tcbp = %0" : : "r"(entry->tcb)); // keep in sync w/ tcb_set in pthread_md.h
  avm2_self_lock();
  start_func(start_func_arg);
  avm2_self_unlock();
}

int thr_new(struct thr_param *param, int param_size) __attribute__((noinline));
int thr_new(struct thr_param *param, int param_size)
{
  static int sTID = -1;
  ThreadEntry *entry;
  int tid;
  int isUIThread;

  if(param_size != sizeof(struct thr_param))
  {
    errno = EINVAL;
    return -1;
  }
  if((param->flags & ~(THR_SYSTEM_SCOPE)) != 0) // suspended is handled by pthreads and a condition
  {
    errno = EINVAL;
    return -1;
  }

  if(sTID < 0)
    sTID = uiThreadIdRaw(0); // start counting AFTER any uiThreadId (we pre-inc sTID, so no +1 needed here)

  // identify the ui thread by its stack
  isUIThread = uiThreadESPInit(NULL) == ((char *)param->stack_base + param->stack_size);

  // check for too many workers (unless we're creating the UI thread)
  if(!isUIThread && flascc_maxWorkers > 0)
  {
    int numWorkers;

    __asm("%0 = workerDomainClass.current.listWorkers().length" : "=r"(numWorkers));

    if(numWorkers >= flascc_maxWorkers) 
    {
      errno = EPROCLIM;
      return -1;
    }
  }

  // ensure we have a thread entry
  ensureSelfThrEntry();

  // ensure self lock set up for threading
  avm2_self_lock();
  avm2_self_unlock();

  avm2_is_ui_worker(); // ensure is_ui state is set up by entry thread

  // set us as ui thread if no other has been designated
  entry = malloc(sizeof(ThreadEntry));
  entry->id = tid = (isUIThread ? uiThreadId() : __sync_add_and_fetch(&sTID, 1));
  entry->tcb = param->tls_base;
  __asm __volatile__ ("var worker:* = newThread(%0, %1, %2, new <int>[%3, %4, %5]);" :
    : "r"(tid), "r"((char *)param->stack_base + param->stack_size), "r"(thread_run), "r"(entry), "r"(param->start_func), "r"(param->arg));
  avm2_lock(&sThreadListMtx);
  entry->next = sThreadListHead;
  sThreadListHead = entry;
  avm2_unlock(&sThreadListMtx);
  if(param->child_tid)
    *param->child_tid = tid+MIN_TID;
  if(param->parent_tid)
    *param->parent_tid = tid+MIN_TID;
  sIsThreaded = true;
  __asm __volatile__ ("worker.start()");
#if CONC_DBG > 0
  {
    long id;
    thr_self(&id);
    tprintf("(%ld) thr_new(%p, %d) => tid: %d\n", id, param, param_size, tid+MIN_TID);
  }
#endif
  return 0;
}

int thr_self(long *id)
{
  *id = thrId()+MIN_TID;
  return 0;
}

// do actual termination on a worker with the given id
// optionally release a simple lock (usually aux stack lock)
static void workerTerm(int id, unsigned *lock1, unsigned *lock2) __attribute__((noinline));
static void workerTerm(int id, unsigned *lock1, unsigned *lock2)
{
#if CONC_DBG > 1
  long tid;

  thr_self(&tid);
  tprintf("(%ld) workerTerm(%d)\n", tid, id);
#endif
  __asm __volatile__ (
//      "try { "
      "for each(var w:* in workerDomainClass.current.listWorkers()) "
      "if(Number(w.getSharedProperty('flascc.threadId')) == %0) "
      "{ "
      // unlock the lock1
      "if(%1) { CModule.callI(%3, new <int>[%1]) } "
      // unlock the lock2
      "if(%2) { CModule.callI(%3, new <int>[%2]) } "
      // terminate
      "w.terminate(); "
      // wait for worker to term
      "while(w.state == 'running' || w.state == 'new') yield(); "
      // should only ever be one match, but just in case...
      "break; "
      "}"
//      "} catch(e:*) { trace(e) }"
      :: "r"(id), "r"(lock1), "r"(lock2), "r"(avm2_unlock));
}

// call a method on an aux stack
// thread safe but callee must unlock the lock if it won't return!
static void *callOnAuxStack(void *(*proc)(unsigned *, void *), void *arg) __attribute__((noinline));
static void *callOnAuxStack(void *(*proc)(unsigned *, void *), void *arg)
{
  static unsigned sLock = 0;
  static char sAuxStack[65536];
  void *result;

  avm2_lock(&sLock);
  __asm __volatile__ ("%0 = CModule.callI(%2, new <int>[%3, %4], %1)" :
      "=r"(result) : "r"(sizeof(sAuxStack) + sAuxStack), "r"(proc), "r"(&sLock), "r"(arg));
  avm2_unlock(&sLock);
  return result;
}

typedef struct
{
  int id;
  long *state;
  bool remove;
} ThrTermArgs;

static bool thrTerm(int id, long *state, bool remove, unsigned *auxStackLock);
static void *thrTermAuxStackShim(unsigned *lock, void *arg)
{
  ThrTermArgs *args = (ThrTermArgs *)arg;

  return (void *)thrTerm(args->id, args->state, args->remove, lock);
}

// terminate a thread by id, optionally wake an address, optionally remove from thread list
// optionally unlock lock for suicide case
static bool thrTerm(int id, long *state, bool remove, unsigned *auxStackLock)
{
  bool suicide = id == thrRealId();
  ThreadEntry *cur, **pnext = &sThreadListHead;

  ensureSelfThrEntry();
  avm2_lock(&sThreadListMtx);
  // find thread
  cur = sThreadListHead;
  while(cur && cur->id != id)
  {
//tprintf("cur: %d (%d)\n", cur->id);
    pnext = &cur->next;
    cur = cur->next;
  }
  if(cur)
  {
    if(remove)
      *pnext = cur->next;
  }
  avm2_unlock(&sThreadListMtx);
//tprintf("thrTerm %p\n", cur);
  if(cur)
  {
    // we unlock the lock no matter who owns it!!!
    unsigned *thrLock = thrSelfLockTCB(cur->tcb);
    unsigned *thrLockCount;

    // don't release the lock ONLY if we're not killing ourself
    // AND we hold the lock
    if(!suicide && avm2_locked_id(thrLock) == (thrRealId()+1))
      thrLock = NULL;
    else
      thrLockCount = thrSelfLockedCountTCB(cur->tcb);

    if(remove)
      free(cur);
    if(!suicide)
    {
      // ensure the worker doesn't have these!
      avm2_lock(&sThreadListMtx);
      avm2_lock(&skpmallocMtx);
      avm2_lock(&smmapMtx);
      arb_enter();
    }
    else
    {
      if(state) // wake here if killing ourself
      {
        *state = 1; // TID_TERMINATED
        _umtx_op(state, UMTX_OP_WAKE, INT_MAX, 0, 0);
      }
      if(thrLock)
        *thrLockCount = 0;
    }
    // actually terminate, unlocking aux stack lock IFF suicide
//tprintf("about to workerTerm\n");
    workerTerm(id, suicide ? thrLock : NULL, suicide ? auxStackLock : NULL);
//tprintf("workerTermed\n");
    arb_exit();
    avm2_unlock(&smmapMtx);
    avm2_unlock(&sThreadListMtx);
    avm2_unlock(&skpmallocMtx);
    if(state)
    {
      *state = 1; // TID_TERMINATED
      _umtx_op(state, UMTX_OP_WAKE, INT_MAX, 0, 0);
    }
    if(thrLock)
    {
      *thrLockCount = 0;
//tprintf("self unlocking %p %d\n", thrLock, *thrLock);
      avm2_unlock(thrLock);
    }
    return true;
  }
  return false;
}


void thr_exit(long *state)
{
  ThrTermArgs args = { thrId(), state, true }; // remove ThreadEntry
#if CONC_DBG > 0
  tprintf("(%ld) thr_exit(%p)\n", thrId()+MIN_TID, state);
#endif
  // always exit on an aux stack since other stuff might
  // free us, etc.
  callOnAuxStack(thrTermAuxStackShim, &args);
}

int thr_kill(long id, int sig)
{
  ThrTermArgs args = { id-MIN_TID, NULL, false }; // leave ThreadEntry for a thr_exit to get it!
  if(!sig)
    return 0;
//tprintf("thr_kill %ld %d\n", id, sig);
  // kill on aux stack in case it's us!! (maybe we don't need to, though... no "state" to wake)
  if(callOnAuxStack(thrTermAuxStackShim, &args))
    return 0;
  errno = EINVAL;
  return -1;
}

int thr_kill2(pid_t pid, long id, int sig)
{
  if(pid == getpid())
    return thr_kill(id, sig);
  errno = EINVAL;
  return -1;
}

// pthreads suspend/resume don't use underlying thr_suspend/thr_resume
int thr_suspend(const struct timespec *timeout)
{
  errno = ENOSYS;
  return -1;
}

int thr_wake(long id)
{
  errno = ENOSYS;
  return -1;
}

int thr_set_name(long id, const char *name)
{
  //TODO?
  return 0;
}

void _thr_rtld_init()
{
//TODO
}

void _thr_rtld_fini()
{
//TODO
}

int __sys_sigtimedwait()
{
  return 0;//TODO
}

int __sys_sigwaitinfo()
{
  return 0;//TODO
}

int tprintf(const char *fmt, ...)
{
  static volatile unsigned sEntered[256] = { 0 };
  long id;
  unsigned cas;

  char buf[16384];
  va_list ap;
  int result;

  // don't re-enter!
  if(avm2_haveWorkers())
  {
    thr_self(&id);
    id %= (sizeof(sEntered) / sizeof(unsigned)); // re-entrancy false-positives if >= 256 threads
    __asm __volatile__ ("%0 = ram.atomicCompareAndSwapIntAt(%1, 0, 1)" : "=r"(cas) : "r"(sEntered + id) : "memory");
  }
  else
    cas = 0;
  if(!cas)
  {
    va_start(ap, fmt);
    result = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if(result > 0 && buf[result-1] == '\n') // crude attempt to make more like printf
        result--;
    if(result > 0)
      __asm __volatile__ ("trace(CModule.readString(%0, %1))" :: "r"(buf), "r"(result));
    sEntered[id] = 0;
  }
  else
    result = -1;
  return result;
}

// cooperative thread arbitration for locks, memory sleeps, etc.

// lock the arbitration lock (recursive ok)
ARB void arb_enter()
{
#if CONC_DBG > 1
  long tid;
  thr_self(&tid);
  tprintf("(%ld) arb_enter...\n", tid);
#endif
 if(sIsThreaded)
    __asm __volatile__("threadArbMutexLock()");
#if CONC_DBG > 1
  tprintf("(%ld) arb_enter!\n", tid);
#endif
}

// unlock the arb lock
ARB void arb_exit()
{
#if CONC_DBG > 1
  long tid;
  thr_self(&tid);
  tprintf("(%ld) arb_exit!\n", tid);
#endif
  if(sIsThreaded)
    __asm __volatile__("threadArbMutexUnlock()");
}

// signal all arbs
ARB void arb_signal(unsigned mask)
{
#if CONC_DBG > 1
  long tid;
  thr_self(&tid);
  tprintf("(%ld) arb_signal!\n", tid);
#endif
  if(sIsThreaded)
  {
    __sync_synchronize();
    __asm __volatile__("threadArbCondsNotify(%0)" : : "r"(mask));
  }
}

// wait on the arbitration condition
// returns true on signal, false on timeout
ARB bool arb_wait(int timo)
{
  bool result;
  double dtimo;

#if CONC_DBG > 1
  long tid;
  thr_self(&tid);
  tprintf("(%ld) arb_wait(%d)\n", tid, timo);
#endif
  if(!sIsThreaded)
    return false;
  if(!timo)
    dtimo = -1;
  else
    dtimo = timo;

  // cap at 3s wait
  if(dtimo < 0 || dtimo > 3000)
    dtimo = 3000;

  __sync_synchronize();
  __asm __volatile__ ("%0 = threadArbCondWait(%1);" : "=r"(result) : "r"(dtimo) : "memory");
  __sync_synchronize();
#if CONC_DBG > 1
  tprintf("(%ld) arb_wait => %d\n", tid, result);
#endif
  return result;
}

// arbitration request structure -- used to resolve
// contested locks, msleep/wake, etc.
typedef struct _s_ArbRequest
{
  struct _s_ArbRequest *next, // next entry
    *tmpNext; // used for temporary linked lists
  int request; // request (ARB_xxx)
  void *addr; // addr MSLEEP-ing on
  unsigned *mtx; // mutex for either LOCK or MSLEEP
  unsigned id; // id for mutex
  int threadId; // requester's thread id
} ArbRequest;

// possible arbitration requests
enum
{
  ARB_COMPLETE, // request serviced
  ARB_LOCK, // requesting a mutex lock
  ARB_MSLEEP // requesting sleep on an address
};

// singly linked list of arbitration requests -- worst case is one per thread
// this var + all linked ArbRequests are guarded by arb_enter/arb_exit
static volatile ArbRequest * volatile sArbRequestHead = NULL;

// ms since epoch in double
ARB double arb_now()
{
  double result;
  __asm __volatile__("%0 = (new Date).time" : "=r"(result));
  return result;
}

// remove a request from the list
ARB void arb_rremoveLocked(ArbRequest *req)
{
  volatile ArbRequest *cur, **pnext = &sArbRequestHead;
  cur = sArbRequestHead;
  while(cur)
  {
    if(cur == req)
    {
      *pnext = cur->next;
      break;
    }
    pnext = &cur->next;
    cur = cur->next;
  }
}

// sleep on an address
int avm2_msleep(void *addr, unsigned *mtx, int timo)
{
  double now = arb_now(), until = now + timo;
  volatile ArbRequest req;
  unsigned id = avm2_locked_id(mtx);

  // fail if no threads -- would hang otherwise
  if(!sIsThreaded)
    return 0;

#if CONC_DBG > 0
  long tid;
  thr_self(&tid);
  tprintf("(%ld) avm2_msleep(%p, %p, %d)\n", tid, addr, mtx, timo);
#endif
  req.request = ARB_MSLEEP;
  req.addr  = addr;
  req.mtx = mtx;
  req.id = id;
  req.threadId = thrId();
  arb_enter();
  // insert request
  req.next = sArbRequestHead;
  sArbRequestHead = &req;
  // unlock mutex
  avm2_unlock(mtx);
  // wait for some action!
  while(!timo || now < until)
  {
    if(req.request == ARB_COMPLETE)
      break;
    arb_wait(timo ? (until - now) : 0);
    now = arb_now();
  }
  arb_rremoveLocked(&req);
  // all done with request
  if(req.request != ARB_COMPLETE)
  {
    avm2_lock_id(mtx, id); // need to reacquire it explicitly if we timed out
  }
  arb_exit();
#if CONC_DBG > 0
  tprintf("(%ld) avm2_msleep => %d\n", tid, req.request == ARB_COMPLETE);
#endif
  return req.request == ARB_COMPLETE;
}

#define CONTESTED (0x80000000U)

int avm2_wake_one(void *addr)
{
  volatile ArbRequest *cur, *sleeper = NULL;
  unsigned signalMask = 0;

#if CONC_DBG > 0
  long tid;
  thr_self(&tid);
  tprintf("(%ld) avm2_wakeone(%p)!\n", tid, addr);
#endif
  arb_enter();
  cur = sArbRequestHead;
  // find last sleeper... for fifo property
  while(cur)
  {
    if(cur->request == ARB_MSLEEP && cur->addr == addr)
      sleeper = cur;
    cur = cur->next;
  }
//tprintf("wakeone %p %p\n", addr, cur);
  if(sleeper)
  {
    if(!__sync_fetch_and_or(sleeper->mtx, CONTESTED)) // got it!
    {
      *(volatile unsigned *)sleeper->mtx = sleeper->id;
      __sync_synchronize();
      sleeper->request = ARB_COMPLETE;
      signalMask |= (1 << (sleeper->threadId & 31)); // woke it! remember to signal
    }
    else
    {
      sleeper->request = ARB_LOCK; // trying to lock
      __sync_synchronize();
    }
  }
  if(signalMask)
    arb_signal(signalMask); // fully woke it!
  arb_exit();
  return sleeper != NULL;
}

int avm2_wake(void *addr)
{
  volatile ArbRequest *cur, *prev = NULL;
  bool foundOne = false;
  unsigned signalMask = 0;

#if CONC_DBG > 0
  long tid;
  thr_self(&tid);
  tprintf("(%ld) avm2_wake(%p)!\n", tid, addr);
#endif
  arb_enter();
  cur = sArbRequestHead;
  // walk fwd finding applicable sleepers
  // and build a temporary reversed list
  while(cur)
  {
    if(cur->request == ARB_MSLEEP && cur->addr == addr)
    {
      cur->tmpNext = prev;
      prev = cur;
      foundOne = true;
    }
    cur = cur->next;
  }
//tprintf("wake %p %p\n", addr, cur);
  // walk temporary reversed (fifo) list
  cur = prev;
  while(cur)
  {
    if(!__sync_fetch_and_or(cur->mtx, CONTESTED)) // got it!
    {
      *(volatile unsigned *)cur->mtx = cur->id;
      __sync_synchronize();
      cur->request = ARB_COMPLETE;
      signalMask |= (1 << (cur->threadId & 31)); // woke it! remember to signal
    }
    else
      cur->request = ARB_LOCK; // trying to lock
    cur = cur->tmpNext;
  }
  if(signalMask)
    arb_signal(signalMask);
  else if(foundOne)
    __sync_synchronize();
  arb_exit();
  return foundOne;
}

ARB void arb_unlock(unsigned *mtx)
{
  volatile ArbRequest *cur, *waiter = NULL;
  int waiters = 0;

#if CONC_DBG > 0
  long tid;
  thr_self(&tid);
  tprintf("(%ld) arb_unlock(%p)!\n", tid, mtx);
#endif
  arb_enter();
  cur = sArbRequestHead;
  // ensure fifo
  while(cur)
  {
    if(cur->request == ARB_LOCK && cur->mtx == mtx)
    {
      waiter = cur;
      waiters++;
    }
    cur = cur->next;
  }
  if(waiter)
  {
    // leave contested if more waiters
    *(volatile unsigned *)mtx = (waiters > 1) ? waiter->id | CONTESTED : waiter->id;
    __sync_synchronize();
    waiter->request = ARB_COMPLETE;
    arb_signal(1 << (waiter->threadId & 31)); // wake assignee
  }
  else
  {
    *mtx = 0; // no one owns it
    __sync_synchronize();
  }
  arb_exit();
}

ARB void arb_lock(unsigned *mtx, unsigned id)
{
#if CONC_DBG > 0
  long tid;
  thr_self(&tid);
  tprintf("(%ld) arb_lock(%p, %d)...\n", tid, mtx, id);
#endif

  arb_enter();
  if(!__sync_fetch_and_or(mtx, CONTESTED)) // contest -- if ends up unowned contested, just take it!
  {
    *(volatile unsigned *)mtx = id;
    __sync_synchronize();
  }
  else
  {
    volatile ArbRequest req;

    req.request = ARB_LOCK;
    req.mtx = mtx;
    req.id = id;
    req.threadId = thrId();

    // insert request
    req.next = sArbRequestHead;
    sArbRequestHead = &req;
    for(;;)
    {
      arb_wait(0);
      if(req.request == ARB_COMPLETE)
        break;
    }
    arb_rremoveLocked(&req);
  }
  arb_exit();
#if CONC_DBG > 0
  tprintf("(%ld) arb_lock!\n", tid);
#endif
}

// simple contestable locks

unsigned avm2_locked_id(unsigned *mtx)
{
  // non-volatile because this thing is practically worthless to non-owner
  return *mtx & ~CONTESTED;
}

void avm2_lock_id(unsigned *mtx, unsigned id)
{
  if(__sync_bool_compare_and_swap(mtx, 0, id))
    return; // acquired unowned
  // go to arbitration
  arb_lock(mtx, id);
}

void avm2_lock(unsigned *mtx)
{
  long id;

  thr_self(&id);
  avm2_lock_id(mtx, id);
}

void avm2_unlock(unsigned *mtx)
{
  unsigned state = *(volatile unsigned *)mtx;

  // try trivial unlock but arbitration on failure
  // only transition for a locked lock is from uncontested to contested
  // (owner can't change) so we'll always be contested if we go to the
  // arbitration!
  if((state & CONTESTED) || !__sync_bool_compare_and_swap(mtx, state, 0))
    arb_unlock(mtx);
}

/*
U	_kfree
U	_kgetnanouptime
U	_kmalloc
U	_kmsleep
U	_ktvtohz
U	_kwakeup
U	_kwakeup_one

U	_ksem_close
U	_ksem_destroy
U	_ksem_getvalue
U	_ksem_init
U	_ksem_open
U	_ksem_post
U	_ksem_timedwait
U	_ksem_trywait
U	_ksem_unlink
U	_ksem_wait
*/

// these need to be implemented and fail for gen/sem.c to fall back to pthreads
int ksem_close()
{
  return -1;
}

int ksem_destroy()
{
  return -1;
}

int ksem_getvalue()
{
  return -1;
}

int ksem_init()
{
  return -1;
}

int ksem_open()
{
  return -1;
}

int ksem_post()
{
  return -1;
}

int ksem_timedwait()
{
  return -1;
}

int ksem_trywait()
{
  return -1;
}

int ksem_unlink()
{
  return -1;
}

int ksem_wait()
{
  return -1;
}

// cheesey impls of kernel mutexes
void kmtx_init(unsigned *mtx, ...)
{
  *mtx = 0;
}

void kmtx_lock(unsigned *mtx)
{
  avm2_lock(mtx);
}

void kmtx_unlock(unsigned *mtx)
{
  avm2_unlock(mtx);
}

extern void *kpmalloc(size_t);
extern bool kpfree(void *);

void *kmalloc(size_t size, ...)
{
  void *result;

  avm2_lock(&skpmallocMtx);
  result = kpmalloc(size);
  avm2_unlock(&skpmallocMtx);
//__asm("trace('kpmalloc: ' + %0)" :: "r"(result));
  return result;
}

void kfree(void *p, ...)
{
//__asm("trace('kpfree: ' + %0)" :: "r"(p));
  avm2_lock(&skpmallocMtx);
  kpfree(p);
  avm2_unlock(&skpmallocMtx);
}

void kgetnanouptime(struct timespec *tsp)
{
  int ms;

  __asm __volatile__ ("%0 = (new Date).time - clockStart" : "=r"(ms));
  tsp->tv_sec = ms / 1000;
  tsp->tv_nsec = (ms % 1000) * 1000000;
}

void kwakeup(void *chan)
{
  if(sIsThreaded)
    avm2_wake(chan);
}

void kwakeup_one(void *chan)
{
  if(sIsThreaded)
    avm2_wake_one(chan);
}

int kmsleep(void *chan, void *mtx, int pri, void *wmesg, int timo)
{
  int result = avm2_msleep(chan, mtx, timo);

  return result ? 0 : EWOULDBLOCK;
}

int
ktvtohz(struct timeval *tv)
{
  return tv->tv_sec * 1000 + tv->tv_usec / 1000;
}

void *_kthread_ctor()
{
  void *thread;
  // keep in sync w/ sys/proc.h and struct thread
  thread = calloc(1, sizeof(void *) * 4);
  umtx_thread_init(thread);
  return thread;
}

// called by _tcb_dtor
void _kthread_dtor(void *thread)
{
  umtx_thread_fini(thread);
  free(thread);
}

static void *kcurthread()
{
  void **tcb = (void **)thrTCB();
  void *thread;

  // keep in sync w/ pthread_md.h and struct tcb (tcb[3] => tcb_kthread)
  thread = tcb[3];
  if(!*(long *)thread) // lazily init this
    thr_self((long *)thread);
  return thread;
}

void *__curthread()
{
  return kcurthread();
}

extern int k_umtx_op(void *thread, void *args); // in kern_umtx.c

int _umtx_op(void *obj, int op, u_long val, void *uaddr, void *uaddr2)
{
  struct {
    void *obj;
    int op;
    u_long val;
    void *uaddr;
    void *uaddr2;
  } kargs = { obj, op, val, uaddr, uaddr2 };
  int result;
#if CONC_DBG > 0
  long tid;
  static const char *sOps[] = {
    "UMTX_OP_LOCK",
    "UMTX_OP_UNLOCK",
    "UMTX_OP_WAIT",
    "UMTX_OP_WAKE",
    "UMTX_OP_MUTEX_TRYLOCK",
    "UMTX_OP_MUTEX_LOCK",
    "UMTX_OP_MUTEX_UNLOCK",
    "UMTX_OP_SET_CEILING",
    "UMTX_OP_CV_WAIT",
    "UMTX_OP_CV_SIGNAL",
    "UMTX_OP_CV_BROADCAST",
    "UMTX_OP_WAIT_UINT",
    "UMTX_OP_RW_RDLOCK",
    "UMTX_OP_RW_WRLOCK",
    "UMTX_OP_RW_UNLOCK",
    "UMTX_OP_WAIT_UINT_PRIVATE",
    "UMTX_OP_WAKE_PRIVATE",
    "UMTX_OP_UMUTEX_WAIT",
    "UMTX_OP_UMUTEX_WAKE"
  };


  thr_self(&tid);

  tprintf("(%ld/%p) _umtx_op(%p, %s, %ld, %p, %p)...\n", tid, kcurthread(), obj, sOps[op], val, uaddr, uaddr2);
#endif
  result = k_umtx_op(kcurthread(), &kargs);
#if CONC_DBG > 0
  tprintf("(%ld) _umtx_op => %d\n", tid, result);
#endif
  return result;
}

int rtprio_thread()
{
  return 0;//TODO
}

int __sys_connect()
{
  return 0;//TODO
}

int __sys_sendmsg()
{
  return 0;//TODO
}

int __sys_recvmsg()
{
  return 0;//TODO
}

int __sys_aio_suspend()
{
  return 0;//TODO
}

int __sys_recvfrom()
{
  return 0;//TODO
}

int __sys_sendto()
{
  return 0;//TODO
}

int __sys_accept()
{
  return 0;//TODO
}

int __sys_poll()
{
  return 0;//TODO
}

int __sys_select()
{
  return 0;//TODO
}

int cpuset_getaffinity()
{
  return 0;//TODO
}

int cpuset_setaffinity()
{
  return 0;//TODO
}

int setlogin()
{
  return 0;//TODO
}

typedef struct _s_ThunkReq
{
  void *(*proc)(void *);
  void *argAndRet;
  int tid;
  void *tcb;
  int except; // was there an exception?
  struct _s_ThunkReq *next;
} ThunkReq;

static void (*sOldUiTickProc)() = NULL;
static volatile ThunkReq *sThunkTickHead = NULL;
static volatile unsigned sThunkTickLock = 0;

#define UITHUNK_LOG 0

static char sEnterFrame; // used as a condition

int avm2_wait_for_ui_frame(int timo)
{
  return avm2_self_msleep(&sEnterFrame, timo);
}

static void uiThunkTickProc()
{
  double end;
  // ok to wait if we woke someone
  int waitOk = avm2_wake(&sEnterFrame);

  // call existing proc if any
  if(sOldUiTickProc)
    sOldUiTickProc();

  end = arb_now() + 5000; // 5000ms max

#if UITHUNK_LOG
  tprintf("uiThunkTickProc\n");
#endif
  // run until we're out of requests
  while(arb_now() < end)
  {
    ThunkReq *cur, *prev = NULL;

    // grab and detach any links
    avm2_lock(&sThunkTickLock);
    cur = sThunkTickHead;
    sThunkTickHead = NULL;
    avm2_unlock(&sThunkTickLock);

    if(!cur)
    {
      if(waitOk)
      {
        waitOk = 0;
        avm2_lock(&sThunkTickLock);
        // we ran at least one, so wait up to 1ms for another to req
        // in case there's a tight loop wanting to call us
        if(!sThunkTickHead)
          avm2_msleep(&sThunkTickHead, &sThunkTickLock, 1);
        avm2_unlock(&sThunkTickLock);
        continue;
      }
      else
        break; // done!
    }

    waitOk = 1; // we're running at least one so wait next iteration is ok

#if UITHUNK_LOG
    tprintf("uiThunkTickProc work\n");
#endif
    // reverse links to run in wait order
    while(cur->next)
    {
      ThunkReq *next = cur->next;
      cur->next = prev;
      prev = cur;
      cur = next;
    }
    cur->next = prev;
    // execute
    while(cur)
    {
      ThunkReq *next = cur->next;
#if UITHUNK_LOG
      tprintf("uiThunkTickProc one\n");
#endif
      cur->argAndRet = thrImpersonate(cur->tid, cur->tcb, cur->proc, cur->argAndRet, &cur->except);
      avm2_wake_one(cur);
      cur = next;
    }
#if UITHUNK_LOG
    tprintf("uiThunkTickProc loop end\n");
#endif
  }
#if UITHUNK_LOG
  tprintf("uiThunkTickProc done\n");
#endif
}

void avm2_self_lock()
{
  unsigned *lock = thrSelfLock();
  unsigned realId = thrRealId()+1;
//tprintf("self_lock: %p %d\n", lock, *thrSelfLockedCount());
  if(avm2_locked_id(lock) != realId)
    avm2_lock_id(lock, realId);
  ++*thrSelfLockedCount();
}

void avm2_self_unlock()
{
  unsigned *lock = thrSelfLock();
//tprintf("self_unlock: %p %d %d\n", lock, *lock, *thrSelfLockedCount());
  if(!--*thrSelfLockedCount())
    avm2_unlock(lock);
//tprintf("self_unlocked: %p %d %d\n", lock, *lock, *thrSelfLockedCount());
}

void *avm2_ui_thunk(void *(*proc)(void *), void *arg)
{
  static bool sRegged = false;
  void *result;

  // are we the ui worker? just run it
  if(avm2_is_ui_worker())
  {
    return proc(arg);
  }

  // link us in
  if(!sRegged)
  {
    avm2_lock(&sThunkTickLock);
    if(!sRegged)
    {
      sOldUiTickProc = flascc_uiTickProc;
      for(;;)
      {
        void *cas;
        cas = __sync_val_compare_and_swap(&flascc_uiTickProc, sOldUiTickProc, uiThunkTickProc);
        if(cas == sOldUiTickProc)
          break;
        sOldUiTickProc = (void (*)())cas;
      }
      sRegged = true;
    }
    avm2_unlock(&sThunkTickLock);
  }
  for(;;) // we retry if we meet an exception!
  {
    volatile ThunkReq req = { proc, arg, thrId(), thrTCB() };
    unsigned selfLockCount;
    unsigned *selfLock = thrSelfLock();
    unsigned selfLockId = thrRealId()+1;

    if(selfLockId != avm2_locked_id(selfLock))
      selfLockId = 0;
    else
      selfLockCount = *thrSelfLockedCount();
    
    avm2_lock(&sThunkTickLock);
    // link in
    req.next = sThunkTickHead;
    sThunkTickHead = &req;
    // wake ui thread in case it's waiting
    avm2_wake_one(&sThunkTickHead);
    // release self lock if we hold it
    if(selfLockId)
    {
      *thrSelfLockedCount() = 0;
//tprintf("unlocking self\n");
      avm2_unlock(selfLock);
//tprintf("unlocked self\n");
    }
    // wait for result!
    avm2_msleep(&req, &sThunkTickLock, 0);
    // reacquire self lock if we held it
    if(selfLockId)
    {
//tprintf("relocking self %p %d \n", thrSelfLock(), *thrSelfLock());
      avm2_lock_id(thrSelfLock(), selfLockId); // lock might have changed if thread created!
//tprintf("relocked self\n");
      *thrSelfLockedCount() = selfLockCount;
    }
    avm2_unlock(&sThunkTickLock);
    if(!req.except) // just try again on exception!
    {
      result = req.argAndRet;
      break;
    }
    avm2_wait_for_ui_frame(5); // wait a bit before trying again
  }
  return result;
}

int avm2_self_msleep(void *addr, int timo)
{
  int result;
  unsigned *selfLock = thrSelfLock();
  unsigned selfLockCount = *thrSelfLockedCount();
//tprintf("self_msleep\n");
  *thrSelfLockedCount() = 0;
  result = avm2_msleep(addr, selfLock, timo);
  if(selfLock != thrSelfLock()) // went threaded during sleep? need a new lock
    avm2_lock_id(thrSelfLock(), thrRealId()+1);
  *thrSelfLockedCount() = selfLockCount;
  return result;
}

void AS3_SendMetricString(const char* metric, const char *value) {
    AS3_DeclareVar(metric, String);
    AS3_DeclareVar(value, String);
    AS3_CopyCStringToVar(metric, metric, strlen(metric));
    AS3_CopyCStringToVar(value, value, strlen(value));
    inline_nonreentrant_as3(
        "import com.adobe.flascc.CModule;\n"
        "CModule.sendMetric(metric, value);\n"
    );
}

void AS3_SendMetricInt(const char* metric, int value) {
    AS3_DeclareVar(metric, String);
    AS3_CopyCStringToVar(metric, metric, strlen(metric));
    inline_nonreentrant_as3(
        "import com.adobe.flascc.CModule;\n"
        "CModule.sendMetric(metric, "" + %0);\n"
        : : "r"(value)
    );
}

// sacraficial pthread created essentially
// just for its tcb...
static void *uiThreadInitThreadProc(void *arg)
{
  long id;

  thr_self(&id);
  *(volatile long *)arg = id;
  avm2_wake(arg);
  thr_kill(id, 9); // kills the worker but thread data structures remain
  return NULL; // unreachable
}

// in thrHelpers.c / thrStubs.c
extern void flascc_uiThreadAttrInit(pthread_attr_t *pattr, void *stack, int stackSize);
extern void flascc_uiThreadAttrDestroy(pthread_attr_t *pattr);

// init threading for the ui thread if we're running in the background
// we create a sacrificial pthread and give the data structures to the
// ui thread
static void uiThreadInit()
{
  typedef void *pvoid;

  if(uiThreadId() >= 0 && uiThreadId() != thrRealId())
  {
    pthread_t thread;
    pthread_attr_t attr;
    void *tcbpp;
    int stackSize;
    void *stack = (char *)uiThreadESPInit(&stackSize) - stackSize;
    volatile long id = 0;

    flascc_uiThreadAttrInit(&attr, stack, stackSize);
    __asm("%0 = workerClass.current.getSharedProperty('flascc.uiThread.tcbpp');" : "=r"(tcbpp));
    if(pthread_create(&thread, &attr, uiThreadInitThreadProc, (void *)&id))
    {
      // pthread_create failed -- maybe no pthreads!
      *(volatile pvoid *)tcbpp = (void *)-1;
    }
    else
    {
      while(!id)
        avm2_self_msleep(&id, 1); // race here so we recheck var after 1ms in case we raced
      // get the tcb from the thread... it'll release its self lock (allowing impersonation)
      // when it thr_kills (which really just kills the worker!)
      *(volatile pvoid *)tcbpp = avm2_thr_impersonate(id, thrTCB, NULL);
    }
    // signal startBackground
    arb_enter();
    arb_signal(-1);
    // wait for it to finish handling uiTick on the mini-stack
    while(*(volatile pvoid *)tcbpp)
    {
//tprintf("tcbpp: %p\n", *(volatile pvoid *)tcbpp);
      arb_wait(1); // 1ms wait due to race
    }
    arb_exit();
    flascc_uiThreadAttrDestroy(&attr);
  }
}

static void __avm2_apply3(struct __avm2_retdata *ret, void *f, void *a, int s)
{
  __asm __volatile__ ("ESP = esp; %0 = CModule.callI(%3, CModule.readIntVector(%4, %5)); %1 = edx; %2 = st0" :
    "=r"(ret->eax), "=r"(ret->edx), "=r"(ret->st0) :
    "r"(f), "r"(a), "r"(s));
}

// crazy machinations to ensure LLVM doesn't decide this can't throw...
void __avm2_apply2(struct __avm2_retdata *ret, void *f, void *a, int s)
{
  void (*proc)(struct __avm2_retdata *ret, void *f, void *a, int s);

  __asm("%0 = %1" : "=r"(proc) : "r"(__avm2_apply3));
  proc(ret, f, a, s);
}

int execve(const char *a, char * const *b, char * const *c)
{
  return -1;
}

int statfs(const char *a, struct statfs *b)
{
  return -1;
}

ssize_t	_recvfrom(int a, void *b, size_t c, int d, struct sockaddr * __restrict e, socklen_t * __restrict f)
{
  return -1;
}

int	_getsockname(int a, struct sockaddr * __restrict b, socklen_t * __restrict c)
{
  return -1;
}

int	_getpeername(int a, struct sockaddr * __restrict b , socklen_t * __restrict c)
{
  return -1;
}

int	bind(int a, const struct sockaddr *b, socklen_t c)
{
  return -1;
}

ssize_t	recvfrom(int a, void *b, size_t c, int d, struct sockaddr * __restrict e, socklen_t * __restrict f)
{
  return -1;
}

int	getsockname(int a, struct sockaddr * __restrict b, socklen_t * __restrict c)
{
  return -1;
}

int	getpeername(int a, struct sockaddr * __restrict b , socklen_t * __restrict c)
{
  return -1;
}


