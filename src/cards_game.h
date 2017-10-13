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

#define CARDS 52

enum Suit_Type
{
    Suit_Start,
    Suit_End,
    Suit_None,
    Suit_B1,
    Suit_B2,
    Suit_R1,
    Suit_R2,
    Suit_Empty
};

struct card
{
    Suit_Type Type;
    i32 TextureHandle;
    i32 Rank;
};

struct grid_tile
{
    card Card;
    b32 Walked;
};

struct grid
{
    math::v2 Size;
    r32 TileScale;
    grid_tile** Grid;
};

struct level
{
    grid Grid;
    
    b32 Won;
    i32 CurrentScore;
    i32 TargetScore;
    
    math::v2i StartTile;
    
    math::rgba BackgroundColor;
};

GENERIC_MAP(font_handle, i32, char*, StrCmp, NULL, "%s", STR_ASSIGN, VAL_COPY);

struct game_state
{
    memory_arena TotalArena;
    memory_arena WorldArena;
    
    b32 IsInitialized = false;
    
    entity Entities[MAX_ENTITIES];
    i32 EntityCount;
    
    r32 PrevMouseX;
    r32 PrevMouseY;
    
    i32 CurrentLevel;
    i32 LoadedLevels;
    level Levels[16];
    
    r64 Timers[NUM_TIMERS];
    i32 TimerCount;
    
    card Cards[CARDS];
    i32 CardCount;
    
    i32 BorderTexture;
    i32 StartTexture;
    i32 EndTexture;
    i32 PlayerTexture;
    
    font_handle_map FontMap;
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

