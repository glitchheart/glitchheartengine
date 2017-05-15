#ifndef ANIMATION_H
#define ANIMATION_H

struct sprite_sheet_frame
{
    real32 X;
    real32 Y;
};


struct animation
{
    char Name[255];
    real32 TimePerFrame;
    uint32 TextureHandle;
    uint32 FrameCount;
    uint32 Rows;
    uint32 Columns;
    bool32 Loop;
    uint32 FrameIndex;
    bool32 Playing;
    real64 CurrentTime;
    
    
    // TODO(niels): Maybe create union for sprite and transform animations?
    sprite_sheet_frame* Frames;
};

#endif