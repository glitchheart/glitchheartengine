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
    glm::vec2 FrameSize;
    glm::vec2 Center;
    b32 Loop;
    
    glm::vec2 FrameOffset;
    sprite_sheet_frame* Frames;
};

#endif