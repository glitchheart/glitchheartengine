#ifndef OSX_THREADING
#define OSX_THREADING

#include <libkern/OSAtomic.h>
#include <pthread.h>
#include <semaphore.h>

// Define platform-specific functions
#define SemaphoreHandle sem_t*
#define WRITE_BARRIER OSMemoryBarrier(); _mm_sfence()
#define THREAD_PROC(name) static void* name(void* parameters)
typedef void* (*ThreadProc)(void* parameters);

static inline u32 interlocked_compare_exchange(u32 volatile *ptr, u32 next, u32 original)
{
    if(OSAtomicCompareAndSwapInt((i32)original, (i32)next, (i32* volatile)ptr))
        return original;
    else
        return next;
}

static inline void interlocked_increment(u32 volatile *ptr)
{
    OSAtomicIncrement32((i32 volatile *)ptr);
}

static inline void release_semaphore(SemaphoreHandle& semaphore_handle)
{
    sem_post(semaphore_handle);
}

static inline void wait_for_semaphore(SemaphoreHandle& semaphore_handle)
{
    i32 wait = sem_wait(semaphore_handle);
    i32 shit = 0;
}

static i32 __id_count = 0;
static inline SemaphoreHandle create_semaphore(u32 initial_count, u32 thread_count)
{
    sem_t* sem;
    char buf[256];
    sprintf(buf, "/s_%d", __id_count++);
    sem = sem_open(buf, O_CREAT, 0644, 1);
    
    return(sem);
}

static inline void create_thread(ThreadProc thread_proc, void *parameter)
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_proc, parameter);
}

#include "threading.h"
#include "threading.cpp"

#endif
