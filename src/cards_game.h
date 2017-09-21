#ifndef GAME_H
#define GAME_H

#include "platform.h"
#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

#define DEBUG

#include "rendering.h"
#include "sound.h"
#include "collision.h"
#include "keycontroller.h"

#define CARDS_ASSETS "../cards/assets/"

enum Entity_Type
{
    Entity_Player
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
    Entity_Type Type;
    
    math::v3 Position;
    math::v3 Velocity;
    math::quat Orientation;
    math::v3 Scale;
    
    u32 Flags;
    
    model Model;
};

#define MAX_ENTITIES 30

struct game_state
{
    memory_arena TotalArena;
    memory_arena WorldArena;
    
    b32 IsInitialized = false;
    
    entity Entities[MAX_ENTITIES];
    i32 EntityCount;
    
    r32 PrevMouseX;
    r32 PrevMouseY;
    
    math::v2 Grid;
    r32 GridScale;
    
    sound_effects Sounds;
};

#define FOR_ENT(Label) for(i32 Label = 0; Label < GameState->EntityCount; Label++) 

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



#endif

