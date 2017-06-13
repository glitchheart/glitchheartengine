#ifndef ANIMATION_H
#define ANIMATION_H

struct sprite_sheet_frame
{
    real32 X;
    real32 Y;
};

struct animation_info
{
    uint32 FrameIndex;
    bool32 Playing;
    real64 CurrentTime;
};

struct animation
{
    char* Name;
    real32 TimePerFrame;
    texture* Texture;
    uint32 FrameCount;
    glm::vec2 FrameSize;
    bool32 Loop;
    
    // TODO(niels): Maybe create union for sprite and transform animations?
    sprite_sheet_frame* Frames;
};

#endif