#ifndef THREADING_H
#define THREADING_H

#define MAX_THREADS 8

#ifdef _WIN32
#define COMPLETE_PAST_WRITES_BEFORE_FUTURE_WRITES _WriteBarrier(); _mm_sfence()
#define COMPLETE_PAST_READS_BEFORE_FUTURE_READS _ReadBarrier()
#else
#endif

typedef void (*WorkPtr)(void);

struct SemaphoreHandle
{
    void *handle;
};

struct ThreadHandle
{
    void *handle;
};

struct ThreadInfo
{
    SemaphoreHandle semaphore_handle;
    i32 logical_thread_index;

    void (*work_ptr)(void);
};

struct ThreadState
{
    ThreadInfo thread_info[MAX_THREADS];
    i32 info_count;

    SemaphoreHandle semaphore_handle;

    i32 volatile next_entry_to_do;
    i32 volatile entry_count;
};

struct WorkQueueEntry
{
    WorkPtr work_ptr;
    void *parameters;
};

struct WorkQueue
{
    SemaphoreHandle semaphore_handle;
    i32 volatile next_entry_to_do;
    i32 volatile entry_completion_count;
    i32 entry_count;
};

static unsigned long thread_proc(void *parameters)
{
    ThreadInfo *thread_info = (ThreadInfo*)parameters;

    printf("Thread started!\n");

    platform.wait_for_semaphore(thread_info->semaphore_handle);

    return 0;
}

static void create_thread(ThreadState &thread_state, void *parameters)
{
    ThreadInfo &thread_info = thread_state.thread_info[thread_state.info_count];
    
    thread_info.semaphore_handle = thread_state.semaphore_handle;
    thread_info.logical_thread_index = thread_state.info_count;
    
    ThreadHandle thread_handle = platform.create_thread(0, thread_proc, parameters, 0);
    platform.close_thread_handle(thread_handle);

    thread_state.info_count++;
}

static void init_threadpool(ThreadState &thread_state, i32 thread_count)
{
    i32 initial_count = 0;
    thread_state.semaphore_handle = platform.create_semaphore(initial_count, MAX_THREADS);
    
    for(i32 i = 0; i < thread_count; i++)
    {
	create_thread(thread_state, NULL);
    }
}

static void add_work_queue_entry(WorkQueue *queue)
{
    COMPLETE_PAST_WRITES_BEFORE_FUTURE_WRITES;
    queue->entry_count++;
    platform.release_semaphore(queue->semaphore_handle);
}

#endif

