#ifndef THREADING_CPP
#define THREADING_CPP

static b32 do_next_work_queue_entry(WorkQueue *queue)
{
    b32 should_sleep = false;

    u32 original_next_entry_to_read = queue->next_entry_to_read;

    // Find the next index, increment or wrap around
    u32 new_next_entry_to_read = (queue->next_entry_to_read + 1) % MAX_ENTRIES;

    // make sure there is more work!
    if(original_next_entry_to_read != queue->next_entry_to_write)
    {
	u32 index = interlocked_compare_exchange(&queue->next_entry_to_read, new_next_entry_to_read, original_next_entry_to_read);
	
	if(index == original_next_entry_to_read)
	{
	    WorkQueueEntry entry = queue->entries[index];
	    entry.work_ptr(queue, entry.data);
	    interlocked_increment(&queue->completion_count);
	}	
    }
    else
    {
	should_sleep = true;
    }

    return(should_sleep);
}

THREAD_PROC(thread_proc)
{
    ThreadInfo *thread_info = (ThreadInfo*)parameters;
	
    printf("Thread started!\n");

    for(;;)
    {
	if(do_next_work_queue_entry(thread_info->queue))
	{
	    wait_for_semaphore(thread_info->queue->semaphore_handle);
	}
    }
}

static void make_queue(WorkQueue *queue, u32 thread_count, ThreadInfo *thread_infos)
{
    queue->completion_goal = 0;
    queue->completion_count = 0;
    queue->next_entry_to_write = 0;
    queue->next_entry_to_read = 0;

    u32 initial_count = 0;
    queue->semaphore_handle = create_semaphore(initial_count, thread_count);

    for(u32 index = 0; index < thread_count; index++)
    {
        ThreadInfo *thread_info = &thread_infos[index];
        thread_info->queue = queue;
		create_thread(thread_proc, (void*)thread_info);
    }
}

static void add_entry(WorkQueue *queue, WorkPtr work_ptr, void *data)
{
    u32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % MAX_ENTRIES;

    // Make sure we haven't filled up the buffer
    assert(new_next_entry_to_write != queue->next_entry_to_read);
    
    queue->entries[queue->next_entry_to_write].work_ptr = work_ptr;
    queue->entries[queue->next_entry_to_write].data = data;
    queue->completion_goal++;

    WRITE_BARRIER;

    queue->next_entry_to_write = new_next_entry_to_write;
    release_semaphore(queue->semaphore_handle);
}

static void complete_all_work(WorkQueue *queue)
{
    while(queue->completion_goal != queue->completion_count)
    {
	do_next_work_queue_entry(queue);
    }

    queue->completion_goal = 0;
    queue->completion_count = 0;
}

#endif
