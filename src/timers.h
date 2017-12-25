#ifndef TIMER_MAX
#define TIMER_MAX 256
#endif

#define START_TIMER(Handle) StartTimer(Handle, TimerController)
#define STOP_TIMER(Handle) StopTimer(Handle, TimerController)
#define TIMER_DONE(Handle) TimerDone(Handle, TimerController)

struct timer
{
    r64 CurrentTime;
    r64 TimerMax;
    char* Name;
};

struct timer_controller
{
    timer Timers[TIMER_MAX];
    i32 TimerCount;
};

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

static b32 TimerDone(i32 Handle, timer_controller& TimerController)
{
    return TimerController.Timers[Handle].CurrentTime >= TimerController.Timers[Handle].TimerMax;
}

static void TickTimers(timer_controller& TimerController, r64 DeltaTime)
{
    for(i32 Index = 0; Index < TimerController.TimerCount; Index++)
    {
        TimerController.Timers[Index].CurrentTime += DeltaTime;
    }
}



