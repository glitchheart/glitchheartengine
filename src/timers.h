#ifndef TIMERS_H
#define TIMERS_H
#ifndef TIMER_MAX
#define TIMER_MAX 1024
#endif

#define REGISTER_TIMER(TimerMax) register_timer(TimerMax, timer_controller)
#define START_TIMER(Handle) start_timer(Handle, timer_controller)
#define STOP_TIMER(Handle) stop_timer(Handle, timer_controller)
#define TIMER_DONE(Handle) timer_done(Handle, timer_controller)
#define CURRENT_TIME(Handle) get_current_timer_time(Handle, timer_controller)
#define MAX_TIME(Handle) get_max_timer_time(Handle, timer_controller)
#define RESET_TIMERS() reset_timers(timer_controller)
#define PAUSE_TIMERS() pause_timers(timer_controller)
#define RESUME_TIMERS() resume_timers(timer_controller)

typedef void* CallbackData;
typedef void (*TimerCallback)(CallbackData data);

struct TimerHandle
{
    i32 handle;
};

struct Timer
{
    b32 running;
    r64 current_time;
    r64 timer_max;
    char* name;
    TimerCallback callback;
    CallbackData callback_data;
};

struct TimerController
{
    b32 paused;
    Timer timers[TIMER_MAX];
    i32 timer_count;
};

static void reset_timers(TimerController& timer_controller)
{
    timer_controller.paused = false;
    timer_controller.timer_count = 0;
}

static TimerHandle register_timer(r64 timer_max, TimerController& timer_controller, TimerCallback callback = nullptr, CallbackData data = nullptr)
{
    i32 handle = timer_controller.timer_count++;
    
    timer_controller.timers[handle].current_time = 0.0f;
    timer_controller.timers[handle].timer_max = timer_max;
    
    timer_controller.timers[handle].callback = callback;
    timer_controller.timers[handle].callback_data = data;
    timer_controller.timers[handle].running = true;
    
    assert(timer_controller.timer_count <= TIMER_MAX);

    return { handle + 1 };
}

static void start_timer(TimerHandle handle, TimerController& timer_controller)
{
    timer_controller.timers[handle.handle - 1].current_time = 0.0f;
}

static void stop_timer(TimerHandle handle, TimerController& timer_controller)
{
    timer_controller.timers[handle.handle - 1].current_time = timer_controller.timers[handle.handle - 1].timer_max;
}

static r64 get_current_timer_time(TimerHandle handle, TimerController& timer_controller)
{
    return timer_controller.timers[handle.handle - 1].current_time;
}

static r64 get_max_timer_time(TimerHandle handle, TimerController& timer_controller)
{
    return timer_controller.timers[handle.handle - 1].timer_max;
}

static b32 timer_done(TimerHandle handle, TimerController& timer_controller)
{
    i32 real_handle = handle.handle - 1;
    if(real_handle < 0 || real_handle > timer_controller.timer_count)
        return true;
    return timer_controller.timers[real_handle].current_time >= timer_controller.timers[real_handle].timer_max;
}

static void tick_timers(TimerController& timer_controller, r64 delta_time)
{
    if(!timer_controller.paused)
    {
        for(i32 index = 0; index < timer_controller.timer_count; index++)
        {
	    Timer &timer = timer_controller.timers[index];
	    if(timer.running)
	    {
		if(timer.current_time >= timer.timer_max)
		{
		    timer.running = false;
		    
		    if(timer.callback)
		    {
			timer.callback(timer.callback_data);
			timer.callback = nullptr;
			timer.callback_data = nullptr;
		    }
		}
		
		timer.current_time += delta_time;
	    }
        }
    }
}

static void pause_timers(TimerController& timer_controller)
{
    timer_controller.paused = true;
}

static void resume_timers(TimerController& timer_controller)
{
    timer_controller.paused = false;
}

#endif
