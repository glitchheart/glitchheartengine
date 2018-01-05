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

struct timer
{
    r64 CurrentTime;
    r64 TimerMax;
    char* Name;
    b32 Delete; // Not used yet, but will be used for array sorting and array cleanup
};

struct timer_controller
{
    b32 Paused;
    timer Timers[TIMER_MAX];
    i32 TimerCount;
};

static void ResetTimers(timer_controller& TimerController)
{
    TimerController.Paused = false;
    TimerController.TimerCount = 0;
}

static void RegisterTimer(i32* Handle, r64 TimerMax, timer_controller& TimerController)
{
    *Handle = TimerController.TimerCount++;
    TimerController.Timers[*Handle].CurrentTime = 0.0f;
    TimerController.Timers[*Handle].TimerMax = TimerMax;
    Assert(TimerController.TimerCount <= TIMER_MAX);
}

static void StartTimer(i32 Handle, timer_controller& TimerController)
{
    TimerController.Timers[Handle].CurrentTime = 0.0f;
}

static void StopTimer(i32 Handle, timer_controller& TimerController)
{
    TimerController.Timers[Handle].CurrentTime = TimerController.Timers[Handle].TimerMax;
}

static r64 GetCurrentTimerTime(i32 Handle, timer_controller& TimerController)
{
    return TimerController.Timers[Handle].CurrentTime;
}

static r64 GetMaxTimerTime(i32 Handle, timer_controller& TimerController)
{
    return TimerController.Timers[Handle].TimerMax;
}

static b32 TimerDone(i32 Handle, timer_controller& TimerController)
{
    return TimerController.Timers[Handle].CurrentTime >= TimerController.Timers[Handle].TimerMax;
}

static void TickTimers(timer_controller& TimerController, r64 DeltaTime)
{
    if(!TimerController.Paused)
    {
        for(i32 Index = 0; Index < TimerController.TimerCount; Index++)
        {
            TimerController.Timers[Index].CurrentTime += DeltaTime;
        }
    }
}

static void PauseTimers(timer_controller& TimerController)
{
    TimerController.Paused = true;
}

static void ResumeTimers(timer_controller& TimerController)
{
    TimerController.Paused = false;
}

#endif