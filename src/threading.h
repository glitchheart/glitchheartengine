#ifndef THREADING_H
#define THREADING_H

#define MAX_THREADS 8
#define MAX_ENTRIES 256

#ifndef SemaphoreHandle
#define SemaphoreHandle void*
#endif

struct WorkQueue;
typedef void (*WorkPtr)(WorkQueue*, void*);

struct WorkQueueEntry
{
    WorkPtr work_ptr;
    void *data;
    b32 is_valid;
};

struct WorkQueue
{
    SemaphoreHandle semaphore_handle;

    u32 volatile completion_goal;
    u32 volatile completion_count;
    u32 volatile next_entry_to_write;
    u32 volatile next_entry_to_read;
    u32 volatile next_entry_to_do;

    WorkQueueEntry entries[MAX_ENTRIES];
};

struct ThreadInfo
{
    i32 logical_thread_index;
    WorkQueue *queue;
};

#endif

