#ifndef ANIMATION_H
#define ANIMATION_H

struct sprite_sheet_frame
{
    r32 X;
    r32 Y;
};

struct animation_info
{
    i32 FrameIndex = 0;
    b32 Playing = false;
    r64 CurrentTime = 0;
    b32 FreezeFrame = false;
};

struct animation
{
    char* Name;
    r32 TimePerFrame;
    texture* Texture;
    i32 FrameCount = 0;
    math::v2 FrameSize;
    math::v2 Center;
    b32 Loop;
    
    math::v2 FrameOffset;
    sprite_sheet_frame* Frames;
};

#endif