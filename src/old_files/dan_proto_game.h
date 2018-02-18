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

enum Bullet_Type
{
    Bullet_Simple
};

struct bullet
{
    Bullet_Type Type;
    math::v3 Position;
    math::v3 Velocity;
    math::v3 Direction;
    r32 Speed;
};

struct level_chunk
{
    math::v2* Stars;
    i32 StarCount;
};

struct level
{
    level_chunk** Chunks;
    i32 ChunkWidth;
    i32 ChunkHeight;
};

struct entity
{
    Entity_Type Type;
    
    math::v3 Position;
    math::v3 Rotation;
    math::v3 Velocity;
    
    model Model;
    
    i32 TextureHandle;
    
    union
    {
        struct
        {
            r32 Thrust;
            math::v3 Acceleration;
            Direction CurrentDirection;
            b32 IsMoving;
            r32 MovementSpeed;
            bullet Bullets[100];
            i32 BulletCount;
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
    level Level;
    i32 BulletTextureHandle;
    i32 StarEnemyTextureHandle;
    i32 PixelArtFontHandle;
    memory_arena TotalArena;
    memory_arena WorldArena;
    
    b32 Loaded = false;
    
    r32 TileScale;
    entity Player;
    math::v3 LowestPositionInWorld;
    math::v3 HighestPositionInWorld;
};

#endif