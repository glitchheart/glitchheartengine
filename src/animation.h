#ifndef ANIMATION_H
#define ANIMATION_H

struct spritesheet_frame
{
    r64 Duration;
    r32 X;
    r32 Y;
    u32 FrameWidth;
    u32 FrameHeight;
};

struct spritesheet_animation_info
{
    i32 FrameIndex;
    b32 Playing;
    r64 CurrentTime;
    b32 FreezeFrame;
    i32 AnimationHandle;
};

struct spritesheet_animation
{
    char* Name;
    i32 TextureHandle;
    i32 FrameCount = 0;
    math::v2 FrameSize;
    b32 Loop;
    
    math::v2 FrameOffset;
    spritesheet_frame* Frames;
};

#endif