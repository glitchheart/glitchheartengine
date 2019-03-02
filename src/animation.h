#ifndef ANIMATION_H
#define ANIMATION_H

#define PARAMETER_NAME_LENGTH 32
#define CONDITION_ARRAY_SIZE 8
#define ANIMATION_NODE_SIZE 16

struct AnimationParameter
{
    char name[PARAMETER_NAME_LENGTH];
    b32 value;
};

struct AnimationLinkCondition
{
    i32 parameter_handle;
    b32 expected_value;
};

#define ANIMATION_CALLBACK(name) void name(void* state, void* data, Renderer *renderer, sound::SoundSystem* sound_system, InputController* input_controller, TimerController& timer_controller)

typedef ANIMATION_CALLBACK(animation_callback);

struct AnimationCallbackInfo
{
    animation_callback* callback;
    i32 frame;
    
    b32 was_called;
    void* state;
    void* data;
};

struct AnimationNodeLink
{
    i32 origin_node;
    i32 destination_node;
    
    b32 after_finished_animation;
    AnimationLinkCondition conditions[CONDITION_ARRAY_SIZE];
    i32 condition_count;
    
    b32 condition_met;
};

#define MAX_ANIMATION_LINKS 8
#define MAX_ANIMATION_CALLBACKS 8

struct AnimationNode
{
    char name[32];
    i32 animation_handle;
    
    b32 loop;
    b32 freeze_on_last_frame;
    
    AnimationNodeLink links[MAX_ANIMATION_LINKS];
    i32 link_count;
    
    AnimationCallbackInfo callback_infos[MAX_ANIMATION_CALLBACKS];
    i32 callback_info_count;
};

#define MAX_ANIMATION_NODES 16
#define MAX_ANIMATION_PARAMETERS 8

struct AnimationController
{
    i32 current_frame_index;
    r64 current_time;
    b32 playing;
    
    r32 speed;
    
    i32 current_node;
    AnimationNode nodes[MAX_ANIMATION_NODES];
    i32 node_count;
    
    AnimationParameter parameters[MAX_ANIMATION_PARAMETERS];
    i32 parameter_count;
};

struct SpritesheetFrame
{
    r64 duration;
    r32 x;
    r32 y;
    i32 frame_width;
    i32 frame_height;
};

struct SpritesheetAnimation
{
    char name[32];
    i32 texture_handle;
    i32 frame_count = 0;
    math::Vec2 frame_size;
    
    math::Vec2 frame_offset;
    SpritesheetFrame* frames;
};

#endif
