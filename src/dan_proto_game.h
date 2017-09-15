#ifndef GAME_H
#define GAME_H

#include "platform.h"
#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

#include "rendering.h"
#include "sound.h"
#include "collision.h"
#include "keycontroller.h"

enum Entity_Type
{
    Player,
    Box
};

enum Direction
{
    None,
    Up,
    Down,
    Left,
    Right
};

struct entity
{
    Entity_Type Type;
    
    math::v3 Position;
    math::v3 Velocity;
    
    model Model;
    
    union
    {
        struct
        {
            union
            {
                math::v3 SwordSizes[2];
                struct
                {
                    math::v3 HorizontalSize;
                    math::v3 VerticalSize;
                };
            };
            union
            {
                math::v3 SwordOffset[4];
                struct
                {
                    math::v3 UpOffset;
                    math::v3 DownOffset;
                    math::v3 LeftOffset;
                    math::v3 RightOffset;
                };
            };
            
            Direction CurrentDirection;
            b32 IsMoving;
            
            math::v2i TargetTile;
            math::v2i CurrentTile;
            math::v3 Offset;
            math::v3 Size;
            r32 MovementSpeed;
        } Player;
        struct
        {
            math::rgba Color;
            math::v2i CurrentTile;
            math::v3 Size;
        } Box;
    };
};

struct game_state
{
    memory_arena TotalArena;
    memory_arena WorldArena;
    
    b32 Loaded = false;
    
    r32 TileScale;
    entity Player;
    entity Boxes[5];
};

#endif