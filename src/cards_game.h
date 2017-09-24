#ifndef GAME_H
#define GAME_H

#include "platform.h"
#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

//#define DEBUG

#include "rendering.h"
#include "sound.h"
#include "collision.h"
#include "keycontroller.h"

#define CARDS_ASSETS "../cards/assets/"

enum EType
{
    EType_Player
};

enum Entity_Flags
{
    EFlag_Active = (1 << 0),
    EFlag_Dead = (1 << 1),
    EFlag_IsKinematic = (1 << 2),
    EFlag_IsTemporary = (1 << 3),
    EFlag_IsStatic = (1 << 4),
};

struct entity
{
    EType Type;
    
    math::v3 Position;
    math::v3 Velocity;
    math::quat Orientation;
    math::v3 Scale;
    
    union
    {
        struct
        {
            r32 Speed;
            timer MoveTimer;
        } Player;
    };
    
    u32 Flags;
};

#define MAX_ENTITIES 30
#define NUM_TIMERS 64

#define GRID_X 5
#define GRID_Y 5
#define CARDS 9

struct grid_tile
{
    i32 Type;
    i32 Walked;
};

struct grid
{
    math::v2 Size;
    r32 TileScale;
    grid_tile Grid[GRID_X][GRID_Y];
    math::v3 GridOffset;
};

struct game_state
{
    memory_arena TotalArena;
    memory_arena WorldArena;
    
    b32 IsInitialized = false;
    
    entity Entities[MAX_ENTITIES];
    i32 EntityCount;
    
    r32 PrevMouseX;
    r32 PrevMouseY;
    
    grid Grid;
    
    r64 Timers[NUM_TIMERS];
    i32 TimerCount;
    
    sound_effects Sounds;
};

#define FOR_ENT(Label) for(i32 Label = 0; Label < GameState->EntityCount; Label++) 
#define GET_ENT(e, EntType_) ((e)->Type == EType_##EntType_ ? &(e)->EntType_ : 0)

inline b32
IsSet(entity *Entity, u32 Flag)
{
    b32 Result = Entity->Flags & Flag;
    
    return(Result);
}

inline void
AddFlags(entity *Entity, u32 Flag)
{
    Entity->Flags |= Flag;
}

inline void
ClearFlags(entity *Entity, u32 Flag)
{
    Entity->Flags &= ~Flag;
}



void StartTimer(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle == -1)
    {
        Timer.TimerHandle = GameState->TimerCount;
        GameState->TimerCount++;
        Assert(GameState->TimerCount < NUM_TIMERS);
    }
    
    GameState->Timers[Timer.TimerHandle] = Timer.TimerMax;
}

b32 TimerDone(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle == -1)
        return true;
    
    return GameState->Timers[Timer.TimerHandle] <= 0;;
}

r64 ElapsedTimer(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle == -1)
        return 1.0;
    return GameState->Timers[Timer.TimerHandle];
}

r64 ElapsedTimerFrac(game_state* GameState, timer& Timer)
{
    return ElapsedTimer(GameState,Timer) / Timer.TimerMax;
}


static inline void TickTimers(game_state* GameState, r64 DeltaTime)
{
    for(u32 Index = 0; Index < NUM_TIMERS; Index++)
    {
        if(GameState->Timers[Index] > 0)
            GameState->Timers[Index] -= DeltaTime;
        else
        {
            GameState->Timers[Index] = 0;
        }
    }
}


#endif

