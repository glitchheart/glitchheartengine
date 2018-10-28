#ifndef THREADING_H
#define THREADING_H

#define MAX_THREADS 8

typedef void (*WorkPtr)(void);

#ifndef SemaphoreHandle
#define SemaphoreHandle u32;
#endif

struct WorkQueueEntry
{
	WorkPtr work_ptr;
	void *data;
	
	b32 is_valid;
};

struct WorkQueueEntryStorage
{
	WorkPtr work_ptr;
    void *data;
};

struct WorkQueue
{
	SemaphoreHandle semaphore_handle;
	u32 volatile next_entry_to_do;
    u32 volatile entry_completion_count;

	WorkQueueEntryStorage entries[256];
    u32 volatile entry_count;
};

struct ThreadInfo
{
    i32 logical_thread_index;
	WorkQueue *queue;
};

static b32 queue_work_still_in_progress(WorkQueue *queue)
{
	b32 result = queue->entry_count != queue->entry_completion;
	return(result);
}
#endif

