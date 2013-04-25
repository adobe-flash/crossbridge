/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_VMPI__
#define __avmplus_VMPI__

// Belt and suspenders...

#ifdef DEBUG
    #ifndef _DEBUG
        #define _DEBUG
    #endif
#endif

#ifdef _DEBUG
    #ifndef DEBUG
        #define DEBUG
    #endif
#endif

#ifdef NDEBUG
#undef _DEBUG
#undef DEBUG
#endif //#ifdef NDEBUG

/////////
// TODO: Remove dependency of VMPI on the feature system
#ifdef AVMSHELL_BUILD
#include "../shell/avmshell-features.h"
#else
/* The embedding host must provide this file in some directory that's included in
 * header search paths.  It must define a value (0 or 1) for every feature
 * required by avmplus, see shell/avmshell-features.h for a model, also see
 * core/avmfeatures.as for documentation of the feature system.
 */
#include "avmhost-features.h"
#endif

// Include the feature system here so that the platform files can depend on
// the internal (VMCFG_, etc) names rather than feature names.  Eases
// maintainability.

#include "../core/avmfeatures.h"
// END TODO: remove dependency of VMPI on the feature system
/////////

// TODO Move this file to VMPI:
#include "system-selection.h"

// TODO Move these files to VMPI:
#if AVMSYSTEM_WIN32
  #include "win32/win32-platform.h"
#elif AVMSYSTEM_UNIX
  #include "unix/unix-platform.h"
#elif AVMSYSTEM_MAC
  #include "mac/mac-platform.h"
#elif AVMSYSTEM_SYMBIAN
  #include "symbian/symbian-platform.h"
#endif

// Catchall, though in general the platform files are really responsible for giving
// REALLY_INLINE and FASTCALL a definition.

#ifndef REALLY_INLINE
    #define REALLY_INLINE inline
#endif
#ifndef FASTCALL
    #define FASTCALL
#endif

// Bug 569361.  NO_INLINE is hint that annotated function should be kept
// out-of-line.  Caveats: the NO_INLINE annotations guarantee nothing,
// as is evident from catch-all definition below (also, some compilers
// may require it to be applied to declaration sites to be effective).
#ifndef NO_INLINE
    #define NO_INLINE
#endif

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/**
 * This method is used to get the size of the memory page of the system
 * @return return the size, if bytes, of memory page
 */
extern size_t       VMPI_getVMPageSize();

/**
* This method should return the difference in milliseconds between local time and UTC
* @return offset in milliseconds
*/
extern double       VMPI_getLocalTimeOffset();

/**
* This method should return the Daylight Savings time adjustment in milliseconds
* @return number of milliseconds corresponding to daylight savings adjustment when active, 0 otherwise
*/
extern double       VMPI_getDaylightSavingsTA(double time);

/**
* This method should return the current UTC date and time in milliseconds
* @return UTC date and time in milliseconds
*/
extern double       VMPI_getDate();

/**
* This method should return the system time in milliseconds
* The implementation of this method could either return the time elapsed since the system started or since epoc
* @return number of milliseconds elapsed
*/
extern uint64_t     VMPI_getTime();

/**
* This method is called to output log messages
* The implementation of this method is platform-defined
*  @param message NULL-terminated UTF8-encoded string
* @return none
*/
extern void VMPI_log(const char* message);

/**
* This method is called to output debugging messages
* This method is specifically for debugging purposes and is invoked to output useful debug informaton
* The implementation of this method is platform-defined.  For example, it could be used to output the message to an IDE
* @param message NULL-terminated UTF8-encoded string
* @return none
*/
extern void VMPI_debugLog(const char* message);

/**
* This method is called to interrupt the program execution during debugging
* Upon a call to this method, the platform execution should interrupt the program execution
* while maintaining the current call stack and associated program information such as the registers
* and return the control back to the user/developer
* This method is specifically for debugging purposes and is invoked to enter a debug interrupt to examine information like the variable values or call stack
* The implementation of this method is platform-defined.
* On platforms that cannot support debug interrupts the program execution should be aborted
* @return none
*/
extern void VMPI_debugBreak();

/**
* This method is used to request a block of memory from the system
* @param size size, in bytes, of memory block requested
* @return pointer to the start of memory block if allocation was successful, NULL otherwise
*/
extern void*        VMPI_alloc(size_t size);

/**
* This method is used to free a previously allocated block
* @param ptr pointer to the memory that needs to be released
* @return none
*/
extern void         VMPI_free(void* ptr);


/**
* This method is used to free a previously allocated block
* @param ptr pointer to the memory that needs to be released
* @return none
*/
extern size_t           VMPI_size(void* ptr);


/**
* Method to get the frequency of a high performance counter/timer on the system
* On platforms where no API to retrieve this information should return a number that closely
* matches its timer frequency
* @return 64-bit value indicating the frequency (i.e. counts per second) of the system's performance counter or clock with highest resolution
* @see VMPI_getPerformanceCounter()
*/
extern uint64_t     VMPI_getPerformanceFrequency();

/**
* Method to get the current value of the system's performance counter/timer
* Platforms that support a high performance counter should return its current value
* Platforms that do not have a high performance counter should return the current timer/clock
* value that was used as a basis to calculate the frequency returned from VMPI_getPerformanceFrequency()
* @return 64-bit value indicating the current value of the counter
* @see VMPI_getPerformanceFrequency()
*/
extern uint64_t   VMPI_getPerformanceCounter();

/* A client of VMPI_startTimer */
class VMPI_TimerClient
{
public:
    // a function that will be called on every tick.
    virtual void tick() = 0;
    // KP: lack of the following causes compilation warnings (treated as errors).
    virtual ~VMPI_TimerClient() {}
};

/**
 * Start a timer that will call into a client object periodically. You can create as many
 * timers as you like.
 * @param period the number of microseconds between calls. We don't guarantee this,
 * you might get called early or late, or the actual period may be different from what you wanted.
 * @param client an object which contains a function that we will call on every tick
 * @return an id for this timer. You can pass this id to VMPI_stopTimer when you want to stop.
 */
extern uintptr_t    VMPI_startTimer(unsigned int period, VMPI_TimerClient* client);

/**
 * Stop a timer.
 * @param id the id for this timer, obtained when VMPI_startTimer was called
 * @return none
 */
extern void         VMPI_stopTimer(uintptr_t id);

/**
* Method to create a new instance of vmpi_spin_lock_t
* This instance will subsequently be passed to acquire/release lock methods
* @return newly created vmpi_spin_lock_t instance
*/
extern void         VMPI_lockInit(vmpi_spin_lock_t* lock);

/**
* Method to destroy the vmpi_spin_lock_t instance
* This method is called when MMgc no longer intends to use the vmpi_spin_lock_t
* instance created and return via VMPI_lockCreate.
* The implementation can safely destroy the lock instance.
* It is allowed for the caller to have acquired the lock when this function is called.
* @param lock instance of vmpi_spin_lock_t to be destroyed
* @return none
* @see VMPI_lockCreate
*/
extern void         VMPI_lockDestroy(vmpi_spin_lock_t* lock);

/**
* Method to acquire a lock on a vmpi_spin_lock_t instance
* If this method returns true, MMgc assumes that the lock was acquired successfully
* During a call to this method, if the lock was held by some other thread then the
* implementation should wait until the lock becomes available
* Return value of false is considered to be an error condition and should only happen
* in exception situations
* @param lock instance to acquire the lock on
* @return true if lock was successfully acquired, false in event of failure
*/
extern bool         VMPI_lockAcquire(vmpi_spin_lock_t* lock);

/**
* Method to release a lock on a vmpi_spin_lock_t instance
* @param lock instance to release the lock on
* @return true if lock was successfully release, false in event of failure
*/
extern bool         VMPI_lockRelease(vmpi_spin_lock_t* lock);

/**
* Method to obtain a lock on a vmpi_spin_lock_t instance if it isn't locked
* @param lock instance to release the lock on
* @return true if lock was successfully aqcuired, false if another thread has it
*/
extern bool         VMPI_lockTestAndAcquire(vmpi_spin_lock_t* lock);

/**
 * Method to create a thread local storage (TLS) identifier
 * This identifier will be used as a key to set/get thread-specific data
 * @param [out] pointer to store the value of newly created TLS identifier
 * @return true if TLS identifier was created successfully, false otherwise
*/
extern bool VMPI_tlsCreate(uintptr_t* tlsId);

/**
 * Method to destroy a previously created TLS identifier
 * @param tlsId TLS identifier to be destroyed
 * @return none
 * @see VMPI_tlsCreate()
*/
extern void VMPI_tlsDestroy(uintptr_t tlsId);

/**
 * Method to associate a thread-specific data with a TLS identifier
 * previously created by VMPI_tlsCreate
 * @param tlsId TLS identifier to associate the data with
 * @param value data to be associated with id
 * @return true if value was set successfully, false otherwise
*/
extern bool VMPI_tlsSetValue(uintptr_t tlsId, void* value);

/**
 * Method to retrieve a data associated with a TLS identifier
 * that may have been previously set via VMPI_tlsSetValue
 * @param tlsId TLS identifier for which the associated data should be retrieved
 * @return the associated data, else NULL if no value was set
 * @see VMPI_tlsSetValue
*/
extern void* VMPI_tlsGetValue(uintptr_t tlsId);

/**
 * Obtain current thread identifier
 * @return thread id
 */
extern vmpi_thread_t VMPI_currentThread();

/**
 * Obtain a platform-specific value that is guaranteed to never be returned
 * by neither VMPI_currentThread nor VMPI_threadCreate.
 *
 * This is meant to serve as a replacement for uses of 'NULL' in code
 * that assumed vmpi_thread_t has pointer type.
 *
 * @return value that no thread id will match.
 */
extern vmpi_thread_t VMPI_nullThread();

/**
 * wrapper around getenv function, return's NULL on platforms with no env vars
 * @return value of env var
 */
extern const char *VMPI_getenv(const char *name);

/**
 * Save all registers into the stack and invoke 'fn' in a non-tail fashion, passing it
 * a conservative approximation to the true stack top (the lowest live address) as
 * well as 'arg'.  The hot part of the stack - where the registers are saved - is
 * volatile; once the function returns that part of the stack does not reliably contain
 * the saved registers any longer.  The size of the hot part of the stack is
 * platform-dependent.
 */
extern void VMPI_callWithRegistersSaved(void (*fn)(void* stackPointer, void* arg), void* arg);

/**
 * Atomically increments and returns the value pointed to by 'value'.
 * If the implementation allows, then no memory barrier will be applied.
 *
 * @param value Points to the value to increment
 * @return The incremented value
 */
extern int32_t VMPI_atomicIncAndGet32(volatile int32_t* value);

/**
 * Atomically increments and returns the value pointed to by 'value'.
 * The operation includes a memory barrier with the ordering guarantees
 * of VMPI_memoryBarrier().
 *
 * @param value Points to the value to increment
 * @return The incremented value
 */
extern int32_t VMPI_atomicIncAndGet32WithBarrier(volatile int32_t* value);

/**
 * Atomically decrements and returns the value pointed to by 'value'.
 * If the implementation allows, then no memory barrier will be applied.
 *
 * @param value Points to the value to increment
 * @return The decremented value
 */
extern int32_t VMPI_atomicDecAndGet32(volatile int32_t* value);

/**
 * Atomically decrements and returns the value pointed to by 'value'.
 * The operation includes a memory barrier with the ordering guarantees
 * of VMPI_memoryBarrier().
 *
 * @param value Points to the value to increment
 * @return The decremented value
 */
extern int32_t VMPI_atomicDecAndGet32WithBarrier(volatile int32_t* value);

/**
 * Performs an atomic Compare-And-Swap operation.
 * If the contents at 'address' are equal to 'oldValue', then they
 * are replaced with 'newValue'.
 * If the implementation allows, then no memory barrier will be applied.
 *
 * @param oldValue The value to compare
 * @param newValue The value to swap-in, if oldValue is the current value
 * @param address The address of the value to update
 * @return true if the update was successful
 */
extern bool VMPI_compareAndSwap32(int32_t oldValue, int32_t newValue, volatile int32_t* address);

/**
 * Performs an atomic Compare-And-Swap operation.
 * If the contents at 'address' are equal to 'oldValue', then they
 * are replaced with 'newValue'.
 * The operation includes a memory barrier with the ordering guarantees
 * of VMPI_memoryBarrier().
 *
 * @param oldValue The value to compare
 * @param newValue The value to swap-in, if oldValue is the current value
 * @param address The address of the value to update
 * @return true if the update was successful
 */
extern bool VMPI_compareAndSwap32WithBarrier(int32_t oldValue, int32_t newValue, volatile int32_t* address);

/**
 * Inserts the strongest hardware read/write memory barrier provided by the platform.
 * The minimum guarantee provided by this barrier will be:
 *   - All load and store operations executed before the barrier will appear to
 *     complete before all load and store operations after the barrier.
 *
 * Note that this function inserts an equivalent compiler memory-barrier in addition
 * to the hardware barrier.
 */
extern void VMPI_memoryBarrier();

/**
 * Creates a new thread to start immediate execution at the function 'start_fn',
 * with the argument 'arg'.
 *
 * The thread will be created with attributes described by 'attr'. If attr is NULL
 * then the platform's default attributes will be used. Changing values within attr
 * after the thread has started execution has no effect.
 * Note that it is platform dependent if non-default attributes are honored or completely ignored.
 * As of Nov 2010, support should be:
 *               pthread     win32 (XP version APIs)
 * priority        no         yes
 * stack size      yes        yes
 * guard size      yes        no
 *
 *
 * @param thread On successful thread creation, the thread's id is stored into the location referenced by 'thread'
 * @param attr The attributes with which to start the thread. If NULL, then the platform's default attributes will be used
 * @param start_fn The starting function of the new thread
 * @param arg The single argument to be passed to start_fn
 * @return true if the new thread was successfully created
 */
extern bool VMPI_threadCreate(vmpi_thread_t* thread, vmpi_thread_attr_t* attr, vmpi_thread_start_t start_fn, vmpi_thread_arg_t arg);

/**
 * Indicates to the thread implementation that resources for the given thread
 * can be reclaimed when the thread terminates.
 * After a thread has been detached, any attempt to join the thread has undefined behavior.
 * Multiple detachments of the same thread results in undefined behavior.
 *
 * @param thread The thread to detach
 * @return true if the thread was successfully detached
 */
extern bool VMPI_threadDetach(vmpi_thread_t thread);

/**
 * De-schedules the calling thread for 'timeout_millis' milliseconds.
 * The thread may spuriously awaken before the timeout.
 *
 * @param timeout_millis The length of time to sleep (in milliseconds)
 */
extern void VMPI_threadSleep(int32_t timeout_millis);

/**
 * Causes the calling thread to block until the given thread has terminated.
 * If the given thread has already terminated but is not detached then the function
 * immediately returns.
 * If the given thread has already terminated and been detached then the
 * behavior is undefined.
 * If the given thread is the calling thread then the behavior is undefined.
 * IMPORTANT: This function should not be assumed to be thread safe; the behavior of racing threads
 * wanting to join the same thread is platform dependent.
 *
 * @param thread The thread for the calling thread to join
 */
extern void VMPI_threadJoin(vmpi_thread_t thread);

/**
 * Initializes the vmpi_mutex_t structure referenced by 'mutex' to be used as a recursive mutex.
 * Note that mutex should be allocated by the caller.
 *
 * @param mutex The pre-allocated vmpi_mutex_t to initialize
 * @return true if the mutex was successfully initialized
 */
extern bool VMPI_recursiveMutexInit(vmpi_mutex_t* mutex);

/**
 * Destroys the given mutex.
 * Note that destroying a locked mutex results in undefined behavior
 *
 * @param mutex The mutex to destroy
 * @return true if the mutex was successfully destroyed
 */
extern bool VMPI_recursiveMutexDestroy(vmpi_mutex_t* mutex);

/**
 * Locks the given mutex.
 * If the mutex is currently held by another thread then the calling thread blocks until it is available.
 * The mutex is recursive whereby successive lock operations by the same thread increments the
 * mutex's lock-count. Each subsequent unlock operation decrements the lock-count until reaching zeroing, resulting
 * in an unlocked mutex.
 * The limit and effects of lock-count overflow is dependent on the platform thread implementation.
 *
 * @param mutex The mutex to lock
 */
extern void VMPI_recursiveMutexLock(vmpi_mutex_t* mutex);

/**
 * Identical to VMPI_recursiveMutexLock, except if the mutex is locked by another thread the
 * call returns immediately.
 *
 * @param mutex The mutex to lock
 * @return true if the mutex was successfully acquired or recursively locked.
 */
extern bool VMPI_recursiveMutexTryLock(vmpi_mutex_t* mutex);

/**
 * Unlocks the given mutex.
 * If the mutex is recursively locked then unlocking decrements the lock-count. On reaching a
 * lock-count of zero, the mutex is unlocked.
 * Attempting to unlock a mutex not held by the calling thread results in undefined behaviour.
 *
 * @param mutex The mutex to unlock
 */
extern void VMPI_recursiveMutexUnlock(vmpi_mutex_t* mutex);

/**
 * Initializes the vmpi_condvar_t structure referenced by 'condvar' to be used as a condition variable.
 * Note that condvar should be allocated by the caller.
 *
 * @param condvar The pre-allocated vmpi_condvar_t to initialize
 * @return true if the condition variable was successfully initialized
 */
extern bool VMPI_condVarInit(vmpi_condvar_t* condvar);

/**
 * Destroys the given condition variable.
 * Note that destroying a condition variable which has waiting threads results in undefined behavior.
 *
 * @param condvar The condition variable to destroy
 * @return true if the condition variable was successfully destroyed
 */
extern bool VMPI_condVarDestroy(vmpi_condvar_t* condvar);

/**
 * Blocks the calling thread on the given condition variable.
 * The calling thread must own the given mutex or the results are undefined.
 * When the thread is blocked, it releases its lock on the mutex.
 * The thread remains blocked until the condition variable is signaled
 * (either individually or via a broadcast). Post-wait, the thread will attempt
 * to re-acquire mutex. When the mutex is re-acquired, this function will return.
 *
 * Note that a waiting thread may spuriously awaken without being signaled.
 * Waiting on a single condition variable with multiple mutexes results in undefined behavior.
 * Waiting on a condition variable with a recursively locked mutex results in undefined behavior.
 *
 * @param condvar The condition variable on which to block
 * @param mutex The mutex to release whilst waiting and re-acquire when signaled
 */
extern void VMPI_condVarWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex);

/**
 * Blocks the calling thread on the given condition variable.
 * The calling thread must own the given mutex or the results are undefined.
 * When the thread is blocked, it releases its lock on the mutex.
 * The thread remains blocked until the condition variable is signaled
 * (either individually or via a broadcast), or the specified timeout period has expired.
 * Post-wait, the thread will attempt to re-acquire mutex. When the mutex is re-acquired, this function will return.
 *
 * Note that a waiting thread may spuriously awaken before the timeout period has ended without being signaled.
 * Waiting on a single condition variable with multiple mutexes results in undefined behavior.
 * Waiting on a condition variable with a recursively locked mutex results in undefined behavior.
 *
 * @param condvar The condition variable on which to block
 * @param mutex The mutex to release whilst waiting and re-acquire when signaled
 * @param timeout_millis The maximum amount of time to wait to be signaled
 * @return true if the timeout period expired
 */
extern bool VMPI_condVarTimedWait(vmpi_condvar_t* condvar, vmpi_mutex_t* mutex, int32_t timeout_millis);

/**
 * Signals all threads waiting on the given condition variable.
 * If no threads are waiting then the function simply returns.
 *
 * @param condvar The condition variable to signal
 * @see VMPI_condVarWait
 * @see VMPI_condVarTimedWait
 */
extern void VMPI_condVarBroadcast(vmpi_condvar_t* condvar);

/**
 * Signals a single thread waiting on the given condition variable.
 * If no threads are waiting then the function simply returns.
 * This function does not imply any fairness policy when selecting the thread to wake.
 *
 * @param condvar The condition variable to signal
 * @see VMPI_condVarWait
 * @see VMPI_condVarTimedWait
 */
extern void VMPI_condVarSignal(vmpi_condvar_t* condvar);

/**
 * Initializes the vmpi_thread_attr_t structure referenced by 'attr' with the default
 * values for the platform.
 *
 * @param attr The pre-allocated vmpi_thread_attr_t to initialize
 * @return true if the vmpi_thread_attr_t was successfully initialized
 */
extern bool VMPI_threadAttrInit(vmpi_thread_attr_t* attr);

/**
 * Destroys the vmpi_thread_attr_t structure referenced by 'attr'.
 * Attempting to pass a destroyed vmpi_thread_attr_t to any of the
 * VMPI_threadAttr* functions results in undefined behavior.
 *
 * @param attr The pre-allocated vmpi_thread_attr_t to destroy
 * @return true if the vmpi_thread_attr_t was successfully destroyed
 */
extern bool VMPI_threadAttrDestroy(vmpi_thread_attr_t* attr);

/**
 * Updates the guard-size attribute of the given vmpi_thread_attr_t.
 *
 * Note that on some platforms the size of a thread's guard region
 * is fixed, so passing a non-default value to VMPI_threadCreate via
 * vmpi_thread_attr_t may have no effect. As of Nov' 2010, this is
 * known to be true for win32 (XP API version).
 *
 * @param attr A reference to vmpi_thread_attr_t to update
 * @param size The new guard size
 * @return true if the vmpi_thread_attr_t was successfully updated
 */
extern bool VMPI_threadAttrSetGuardSize(vmpi_thread_attr_t* attr, size_t size);

/**
 * Updates the stack-size attribute of the given vmpi_thread_attr_t.
 *
 * When passed to the VMPI_threadCreate function, the stack-size
 * described vmpi_thread_attr_t will determine the initial
 * stack size allocated to the thread.
 *
 * @param attr A reference to vmpi_thread_attr_t to update
 * @param size The new stack size
 * @return true if the vmpi_thread_attr_t was successfully updated
 */
extern bool VMPI_threadAttrSetStackSize(vmpi_thread_attr_t* attr, size_t size);

/**
 * Updates the priority attribute of the given vmpi_thread_attr_t
 * to 'low-priority'.
 *
 * Note that on some platforms changing the priority of newly started threads
 * will have no effect (as priorities are assigned at the process-level).
 * As of Nov' 2010, this is known to be true for the POSIX platforms.
 *
 * @param attr A reference to vmpi_thread_attr_t to update
 * @return true if the vmpi_thread_attr_t was successfully updated
 */
extern void VMPI_threadAttrSetPriorityLow(vmpi_thread_attr_t* attr);

/**
 * Updates the priority attribute of the given vmpi_thread_attr_t
 * to 'normal-priority'.
 *
 * Note that on some platforms changing the priority of newly started threads
 * will have no effect (as priorities are assigned at the process-level).
 * As of Nov' 2010, this is known to be true for the POSIX platforms.
 *
 * @param attr A reference to vmpi_thread_attr_t to update
 * @return true if the vmpi_thread_attr_t was successfully updated
 */
extern void VMPI_threadAttrSetPriorityNormal(vmpi_thread_attr_t* attr);

/**
 * Updates the priority attribute of the given vmpi_thread_attr_t
 * to 'high-priority'.
 *
 * Note that on some platforms changing the priority of newly started threads
 * will have no effect (as priorities are assigned at the process-level).
 * As of Nov' 2010, this is known to be true for the POSIX platforms.
 *
 * @param attr A reference to vmpi_thread_attr_t to update
 * @return true if the vmpi_thread_attr_t was successfully updated
 */
extern void VMPI_threadAttrSetPriorityHigh(vmpi_thread_attr_t* attr);

/**
 * Returns the platform's default size for stack guards.
 *
 * @return default stack guard size
 */
extern size_t VMPI_threadAttrDefaultGuardSize();

/**
 * Returns the platform's default size for new stacks.
 *
 * @return default stack size
 */
extern size_t VMPI_threadAttrDefaultStackSize();

/**
 * Yield the current processor to another runnable thread if available.
 *
 * Note: It is scheduler implementation dependent what constitutes a 'runnable'
 *       thread, and also what penalty is imposed on the yielding thread, i.e.
 *       when it will be rescheduled. This function should not be used as a
 *       general synchronization mechanism, it is intended to be used only
 *       as an optimization within busy-waiting loops (i.e. yield the processor
 *       immediately, rather than busy-waiting until the thread is pre-empted
 *       by the scheduler).
 *
 * Platform specifics:
 *
 * - Win32 uses SwitchToThread(). From msdn docs:
 *
 *   "The yield of execution is in effect for up to one thread-scheduling time
 *    slice on the processor of the calling thread. The operating system will
 *    not switch execution to another processor, even if that processor is idle
 *    or is running a thread of lower priority.
 *    After the yielding thread's time slice elapses, the operating system
 *    reschedules execution for the yielding thread. The rescheduling is
 *    determined by the priority of the yielding thread and the status of other
 *    threads that are available to run.
 *
 * - All of the POSIX platforms (inc. OSX) use sched_yield(), which is quite
 *   ambiguous as to what scheduling will actually occur.
 *   From The Open Group Base Specifications Issue 6:
 *
 *   "The sched_yield() function shall force the running thread to relinquish
 *    the processor until it again becomes the head of its thread list."
 *
 *
 */
extern void VMPI_threadYield();

/**
 * Returns the number of processors (inc. logical processors) that were available
 * at boot time. Note that the number of dynamically available processors can change
 * (e.g. via power management), so do not rely on this function for algorithm correctness.
 */
extern int VMPI_processorQtyAtBoot();

/**
 * This intrinsic provides a PAUSE instruction for x86 platforms.
 * (It is a NO-OP for all other architectures.)
 *
 * To reduce costly recovery from possible memory ordering violations,
 * Intel Pentium 4's and Xeons benefit from a PAUSE instruction
 * being inserted into spin-wait loops. This serves as a hint
 * to the processor that it is busy-waiting, and to avoid the
 * memory ordering violations.
 *
 * The instruction is ignored by x86 processors that do not require it.
 *
 * See:
 * Intel 64 and IA-32 Architectures Software Developer's Manual Volume 2B 4-71
 */
extern void VMPI_spinloopPause();


/** The internal data structure used by the VMPI_Timer. */
/** Note: this class is only used by the VMPI implementations - GenericPortUtils, WinPortUtils, and PosixPortUtils.
 It doesn't need to be declared in a public header, and should be moved into a private header file. */
struct VMPI_TimerData
{
    void init(uint32_t micros, VMPI_TimerClient *client);

    uint32_t            interval;   // the nominal interval between ticks, in microseconds
    VMPI_TimerClient    *client;    // a object containing a function we must call on every tick
};

#endif /* __avmplus_VMPI__ */
