#ifndef LINUX_THREADING
#define LINUX_THREADING

#include <pthread.h>
#include <semaphore.h>
#include <stdatomic>

// Define platform-specific functions
#define SemaphoreHandle sem_t
#define WRITE_BARRIER __asm__ __volatile__ ("" ::: "memory"); _mm_sfence()
#define THREAD_PROC(name) static void* name(void* parameters)
typedef void* (*ThreadProc)(void* parameters);

static inline u32 interlocked_compare_exchange(u32 volatile *ptr, u32 next, u32 original)
{
    if(atomic_compare_exchange_strong((atomic_int volatile *)ptr, (i32*)&original, (i32)next))
    {
	return original;
    }
    
    return next;
}

static inline void interlocked_increment(u32 volatile *ptr)
{
    atomic_fetch_add((atomic_int volatile *)ptr, 1);
}

static inline void release_semaphore(SemaphoreHandle& semaphore_handle)
{
    sem_post(&semaphore_handle);
}

static inline void wait_for_semaphore(SemaphoreHandle& semaphore_handle)
{
    sem_wait(&semaphore_handle);
}

static inline SemaphoreHandle create_semaphore(u32 initial_count, u32 thread_count)
{
    sem_t sem;
    sem_init(&sem, thread_count, initial_count);
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
