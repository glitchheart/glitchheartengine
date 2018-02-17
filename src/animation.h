#ifndef ANIMATION_H
#define ANIMATION_H

#define PARAMETER_NAME_LENGTH 32
#define CONDITION_ARRAY_SIZE 8
#define ANIMATION_NODE_SIZE 16

struct animation_parameter
{
    char Name[PARAMETER_NAME_LENGTH];
    b32 Value;
};

struct animation_link_condition
{
    i32 ParameterHandle;
    b32 ExpectedValue;
};

struct animation_node_link
{
    i32 OriginNode;
    i32 DestinationNode;
    
    b32 AfterFinishedAnimation;
    animation_link_condition Conditions[CONDITION_ARRAY_SIZE];
    i32 ConditionCount;
};

struct animation_node
{
    char Name[32];
    i32 AnimationHandle;
    
    b32 Loop;
    b32 FreezeOnLastFrame;
    
    animation_node_link Links[8];
    i32 LinkCount;
};

struct animation_controller
{
    i32 CurrentFrameIndex;
    r64 CurrentTime;
    b32 Playing;
    
    r32 Speed;
    
    i32 CurrentNode;
    animation_node Nodes[16];
    i32 NodeCount;
    
    animation_parameter Parameters[32];
    i32 ParameterCount;
};

struct spritesheet_frame
{
    r64 Duration;
    r32 X;
    r32 Y;
    i32 FrameWidth;
    i32 FrameHeight;
};

struct spritesheet_animation
{
    char* Name;
    i32 TextureHandle;
    i32 FrameCount = 0;
    math::v2 FrameSize;
    
    math::v2 FrameOffset;
    spritesheet_frame* Frames;
};

#endif
