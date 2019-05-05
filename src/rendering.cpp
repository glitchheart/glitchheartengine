#include "animation.h"
#include <string.h>

// The InfoHandle is used to be able to reference the same animation without having to load the animation again. 
static void add_animation(Renderer* renderer, SpritesheetAnimation animation, const char* animation_name)
{
    strcpy(animation.name, animation_name);
    renderer->spritesheet_animations[renderer->spritesheet_animation_count++] = animation;
}

static void set_window_mode(Renderer* renderer, i32 resolution_index, WindowMode new_window_mode)
{
    Resolution new_resolution = renderer->available_resolutions[resolution_index];
    renderer->current_resolution_index = resolution_index;
    renderer->api_functions.set_window_mode(renderer->api_functions.render_state, renderer, new_resolution, new_window_mode);
}

static void set_mouse_lock(b32 locked, Renderer &renderer)
{
    renderer.api_functions.set_mouse_lock(locked, renderer.api_functions.render_state);
}

static i32 _find_unused_handle(Renderer& renderer)
{
    for(i32 index = renderer.render._current_internal_buffer_handle; index < global_max_custom_buffers; index++)
    {
        if(renderer.render._internal_buffer_handles[index] == -1)
        {
            renderer.render._current_internal_buffer_handle = index;
            return index;
        }
    }
    
    for(i32 index = 0; index < global_max_custom_buffers; index++)
    {
        if(renderer.render._internal_buffer_handles[index] == -1)
        {
            renderer.render._current_internal_buffer_handle = index;
            return index;
        }
    }
    
    assert(false);
    
    return -1;
}

