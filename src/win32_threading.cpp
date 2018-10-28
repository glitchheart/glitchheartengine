#ifndef WIN32_THREADING
#define WIN32_THREADING

#define SemaphoreHandle HANDLE
#include "threading.h"

static WorkQueueEntry complete_and_get_next_work_queue_entry(WorkQueue *queue, WorkQueueEntry completed)
{
	WorkQueueEntry result;
	result.is_valid = false;

	if(completed.is_valid)
	{
		InterlockedIncrement((LONG volatile *)queue->entry_completion_count);
	}

	u32 original_next_entry_to_do = queue->next_entry_to_do;
	
	if(queue->next_entry_to_do < queue->entry_count)
	{
		u32 index = InterlockedCompareExchange((LONG volatile *)&queue->next_entry_to_do, original_next_entry_to_do + 1, original_next_entry_to_do) - 1;
		if(index == original_next_entry_to_do)
		{
			result.work_ptr = queue->entries[index].work_ptr;
			result.data = queue->entries[index].data;
			result.is_valid = true;
			__ReadBarrier();
		}	
	}
	return(result);
}

static u32 get_next_available_work_queue_index(WorkQueue *queue)
{
	return(queue->entry_count);
}

inline void do_work(WorkQueueEntry entry, i32 logical_thread_index)
{
	assert(entry.is_valid);
	
	if(entry.is_valid)
	{
		entry.work_ptr(entry.data);
	}
}

static unsigned long thread_proc(void *parameters)
{
    ThreadInfo *thread_info = (ThreadInfo*)parameters;
	
    printf("Thread started!\n");

	WorkQueueEntry entry = {};
	for(;;)
	{
		entry = complete_and_get_next_work_queue_entry(thread_info.queue, entry);
		if(entry.is_valid)
		{
			do_work(entry, thread_info->logical_thread_index))
		}
		else
		{
			WaitForSingleObjectEx(thread_info->queue->semaphore_handle, INFINITE, FALSE);
		}
	}

    return 0;
}

static void win32_make_queue(WorkQueue *queue, u32 thread_count, ThreadInfo *thread_infos)
{
	queue->next_entry
	i32 initial_count = 0;
	queue->semaphore_handle = CreateSemaphoreEx(0, initial_count, thread_count, 0, 0, SEMAPHORE_ALL_ACCESS);

    for(u32 index = 0; index < thread_count; index++)
    {
        ThreadInfo *thread_info = &thread_infos[index];
        thread_info->queue = queue;

        DWORD thread_id;
        HANDLE thread_handle = CreateThread(0, 10000000, thread_proc, thread_info, 0, &thread_id);
        CloseHandle(thread_handle);
    }
}

static void init_threadpool(ThreadState &thread_state, i32 thread_count)
{
	thread_state.queue = {};

	i32 initial_count = 0;
	thread_state.queue.semaphore_handle = CreateSemaphoreEx(0, initial_count, MAX_THREADS, 0, 0, SEMAPHORE_ALL_ACCESS);
    
    for(i32 i = 0; i < thread_count; i++)
    {
		create_thread(thread_state, NULL);
    }
}

static void win32_add_work_queue_entry(WorkQueue *queue, WorkPtr *work, void *data)
{
	assert(queue->entry_count < MAX_ENTRIES);

	queue->entries[queue->entry_count].work_ptr = work_ptr;
	queue->entries[queue->entry_count].data = data;

	_WriteBarrier();
	_mm_sfence();
	
	queue->entry_count++;
	
	ReleaseSemaphore(queue->semaphore_handle, 1, 0);
}

static void complete_all_work(WorkQueue *queue)
{
	
}

#endif
