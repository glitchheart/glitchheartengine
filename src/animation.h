#ifndef ANIMATION_H
#define ANIMATION_H

struct sprite_sheet_frame
{
    real32 X;
    real32 Y;
};

struct animation_info
{
    int32 FrameIndex = 0;
    bool32 Playing = false;
    real64 CurrentTime = 0;
    bool32 FreezeFrame = false;
};

struct animation
{
    char* Name;
    real32 TimePerFrame;
    texture* Texture;
    int32 FrameCount = 0;
    glm::vec2 FrameSize;
    glm::vec2 Center;
    bool32 Loop;
    
    glm::vec2 FrameOffset;
    sprite_sheet_frame* Frames;
};

#endif