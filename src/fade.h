#ifndef FADE_H
#define FADE_H

namespace fade
{
    typedef void (*FadeCallback)();

    // fades
    enum FadeMode
    {
        FADE_NONE,
        FADE_OUT,
        FADE_STAY,
        FADE_IN
    };


    struct FadeCommand
    {
        FadeMode mode;
        b32 skippable;
        r64 time;
        r64 current_time;
        FadeCallback callback;
    };

    typedef void (*EnqueueFadeCommand)(FadeState* fade_state, FadeCommand command);

    struct FadeState
    {
        FadeCommand fade_commands[256];
        b32 running_fade_command;
        i32 current_fade_index;
        i32 fade_command_count;
    
        // Fade
        b32 fading;
        FadeMode fade_mode;
        r32 fade_alpha;
        r32 fade_speed;

        EnqueueFadeCommand enqueue_command;
    };

    static void enqueue_fade_command(FadeCommand command)
    {
        core.fade_state->enqueue_command(core.fade_state, command);
    }

    static b32 fading()
    {
        return core.fade_state->fading;
    }

    static FadeMode fade_mode()
    {
        return core.fade_state->fade_mode;
    }
}


#endif
