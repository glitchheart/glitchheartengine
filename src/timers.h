#ifndef TIMERS_H
#define TIMERS_H
#ifndef TIMER_MAX
#define TIMER_MAX 1024
#endif

#define REGISTER_TIMER(HandlePtr, TimerMax) RegisterTimer(HandlePtr, TimerMax, TimerController)
#define START_TIMER(Handle) StartTimer(Handle, TimerController)
#define STOP_TIMER(Handle) StopTimer(Handle, TimerController)
#define TIMER_DONE(Handle) TimerDone(Handle, TimerController)
#define CURRENT_TIME(Handle) GetCurrentTimerTime(Handle, TimerController)
#define MAX_TIME(Handle) GetMaxTimerTime(Handle, TimerController)
#define RESET_TIMERS() ResetTimers(TimerController)
#define PAUSE_TIMERS() PauseTimers(TimerController)
#define RESUME_TIMERS() ResumeTimers(TimerController)

struct Timer
{
    r64 current_time;
    r64 timer_max;
    char* name;
    b32 delete; // Not used yet, but will be used for array sorting and array cleanup
};

struct TimerController
{
    b32 paused;
    Timer timers[TIMER_MAX];
    i32 timer_count;
};

static void reset_timers(TimerController& timer_controller)
{
    timer_controller.Paused = false;
    timer_controller.TimerCount = 0;
}

static void register_timer(i32* handle, r64 timer_max, TimerController& timer_controller)
{
    *handle = TimerController.TimerCount++;
    TimerController.Timers[*handle].CurrentTime = timer_max;
    TimerController.Timers[*handle].TimerMax = timer_max;
    Assert(TimerController.TimerCount <= TIMER_MAX);
}

static void start_timer(i32 handle, TimerController& timer_controller)
{
    timer_controller.Timers[handle].CurrentTime = 0.0f;
}

static void stop_timer(i32 handle, TimerController& timer_controller)
{
    timer_controller.Timers[handle].CurrentTime = timer_controller.Timers[handle].TimerMax;
}

static r64 get_current_timer_time(i32 handle, TimerController& timer_controller)
{
    return timer_controller.Timers[handle].CurrentTime;
}

static r64 get_max_timer_time(i32 handle, TimerController& timer_controller)
{
    return timer_controller.Timers[handle].TimerMax;
}

static b32 timer_done(i32 handle, TimerController& timer_controller)
{
    if(handle < 0 || handle > timer_controller.TimerCount)
        return true;
    return timer_controller.Timers[handle].CurrentTime >= timer_controller.Timers[handle].TimerMax;
}

static void tick_timers(TimerController& timer_controller, r64 delta_time)
{
    if(!timer_controller.Paused)
    {
        for(i32 index = 0; index < timer_controller.TimerCount; index++)
        {
            timer_controller.Timers[index].CurrentTime += delta_time;
        }
    }
}

static void pause_timers(TimerController& timer_controller)
{
    timer_controller.Paused = true;
}

static void resume_timers(TimerController& timer_controller)
{
    timer_controller.Paused = false;
}

#endif
