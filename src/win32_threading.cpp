#ifndef WIN32_THREADING
#define WIN32_THREADING

#define SemaphoreHandle HANDLE
#include "threading.h"

static b32 win32_do_next_work_queue_entry(WorkQueue *queue)
{
    b32 should_sleep = false;

    u32 original_next_entry_to_read = queue->next_entry_to_read;

    // Find the next index, increment or wrap around
    u32 new_next_entry_to_read = (queue->next_entry_to_read + 1) % MAX_ENTRIES;

    // make sure there is more work!
    if(original_next_entry_to_read != queue->next_entry_to_write)
    {
		LONG index = InterlockedCompareExchange((LONG volatile *)&queue->next_entry_to_read, (LONG)new_next_entry_to_read, (LONG)original_next_entry_to_read);
	
		if(index == (LONG)original_next_entry_to_read)
		{
			WorkQueueEntry entry = queue->entries[index];
			entry.work_ptr(queue, entry.data);
			InterlockedIncrement((LONG volatile *)&queue->completion_count);
		}	
    }
    else
    {
		should_sleep = true;
    }

    return(should_sleep);
}

static DWORD thread_proc(void *parameters)
{
    ThreadInfo *thread_info = (ThreadInfo*)parameters;
	
    printf("Thread started!\n");

    for(;;)
    {
		if(win32_do_next_work_queue_entry(thread_info->queue))
		{
			WaitForSingleObjectEx(thread_info->queue->semaphore_handle, INFINITE, FALSE);
		}
    }
}

static void make_queue(WorkQueue *queue, LONG thread_count, ThreadInfo *thread_infos)
{
    queue->completion_goal = 0;
    queue->completion_count = 0;
    queue->next_entry_to_write = 0;
    queue->next_entry_to_read = 0;

    i32 initial_count = 0;
    queue->semaphore_handle = CreateSemaphoreEx(0, initial_count, thread_count, 0, 0, SEMAPHORE_ALL_ACCESS);

    for(long index = 0; index < thread_count; index++)
    {
        ThreadInfo *thread_info = &thread_infos[index];
        thread_info->queue = queue;
	
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(0, 10000000, thread_proc, thread_info, 0, &thread_id);
        CloseHandle(thread_handle);
    }
}

static void win32_add_entry(WorkQueue *queue, WorkPtr work_ptr, void *data)
{
    u32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % MAX_ENTRIES;

    // Make sure we haven't filled up the buffer
    assert(new_next_entry_to_write != queue->next_entry_to_read);
    
    queue->entries[queue->next_entry_to_write].work_ptr = work_ptr;
    queue->entries[queue->next_entry_to_write].data = data;
    queue->completion_goal++;
    
    _WriteBarrier();
    _mm_sfence();
	
    queue->next_entry_to_write = new_next_entry_to_write;

    ReleaseSemaphore(queue->semaphore_handle, 1, 0);
}

static void win32_complete_all_work(WorkQueue *queue)
{
    while(queue->completion_goal != queue->completion_count)
    {
		win32_do_next_work_queue_entry(queue);
    }

    queue->completion_goal = 0;
    queue->completion_count = 0;
}

#endif
