#ifndef WIN32_THREADING
#define WIN32_THREADING

// Define platform-specific functions
#define SemaphoreHandle HANDLE
#define THREAD_PROC(name) static DWORD name(*void)
#define WRITE_BARRIER _WriteBarrier(); _mm_sfence()

static inline u32 interlocked_compare_exchang(u32 *ptr, u32 next, u32 original)
{
    InterlockedCompareExchange(ptr, next, original);
}

static inline void interlocked_increment(u32 *ptr)
{
    InterlockedIncrement((LONG volatile *)ptr);
}

static inline void release_semaphore(SemaphoreHandle semaphore_handle, i32 count)
{
    ReleaseSemaphore(queue->semaphore_handle, count, 0);
}

static inline void wait_for_semaphore(SemaphoreHandle semaphore_handle)
{
    WaitForSingleObjectEx(semaphore_handle, INFINITE, FALSE);
}

static inline SemaphoreHandle create_semaphore(u32 initial_count, u32 thread_count)
{
    return CreateSemaphoreEx(0, initial_count, thread_count, 0, 0, SEMAPHORE_ALL_ACCESS);
}

static inline void create_thread(ThreadProc thread_proc, void *parameter)
{
    DWORD thread_id;
    HANDLE thread_handle = CreateThread(0, 1000000, thread_proc, thread_info, thread_id);
    CloseHandle(thread_handle);
}

#endif

#include "threading.h"
#include "threading.cpp"

#endif
