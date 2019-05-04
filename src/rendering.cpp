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


static void update_lighting_for_material(CombinedCommand &render_command, Renderer *renderer)
{
    rendering::UniformValue *dir_light_count_map = rendering::get_mapping(render_command.material_handle, rendering::UniformMappingType::DIRECTIONAL_LIGHT_COUNT, renderer);
                
    if(dir_light_count_map)
        rendering::set_uniform_value(renderer, render_command.material_handle, dir_light_count_map->name, renderer->render.dir_light_count);

    if(renderer->render.dir_light_count > 0)
    {                    
        rendering::UniformValue *mapping = rendering::get_mapping(render_command.material_handle, rendering::UniformMappingType::DIRECTIONAL_LIGHTS, renderer);
        rendering::UniformValue *light_dir = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::DIRECTIONAL_LIGHT_DIRECTION, renderer);
        rendering::UniformValue *ambient = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::DIRECTIONAL_LIGHT_AMBIENT, renderer);
        rendering::UniformValue *diffuse = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::DIRECTIONAL_LIGHT_DIFFUSE, renderer);
        rendering::UniformValue *specular = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::DIRECTIONAL_LIGHT_SPECULAR, renderer);
                    
        for(i32 light_index = 0; light_index < renderer->render.dir_light_count; light_index++)
        {
            DirectionalLight &light = renderer->render.directional_lights[light_index];

            if(light_dir)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, light_dir->name, light.direction);
            if(ambient)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, ambient->name, light.ambient);
            if(diffuse)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, diffuse->name, light.diffuse);
            if(specular)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, specular->name, light.specular);
        }
    }

    rendering::UniformValue *point_light_count_map = rendering::get_mapping(render_command.material_handle, rendering::UniformMappingType::POINT_LIGHT_COUNT, renderer);
                
    if(point_light_count_map)
        rendering::set_uniform_value(renderer, render_command.material_handle, point_light_count_map->name, renderer->render.point_light_count);

    if(renderer->render.point_light_count > 0)
    {
        rendering::UniformValue *mapping = rendering::get_mapping(render_command.material_handle, rendering::UniformMappingType::POINT_LIGHTS, renderer);
        rendering::UniformValue *light_pos = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_POSITION, renderer);
        rendering::UniformValue *constant = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_CONSTANT, renderer);
        rendering::UniformValue *linear = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_LINEAR, renderer);
        rendering::UniformValue *quadratic = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_QUADRATIC, renderer);
        rendering::UniformValue *ambient = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_AMBIENT, renderer);
        rendering::UniformValue *diffuse = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_DIFFUSE, renderer);
        rendering::UniformValue *specular = rendering::get_array_variable_mapping(render_command.material_handle, mapping->name, rendering::UniformMappingType::POINT_LIGHT_SPECULAR, renderer);
                    
        for(i32 light_index = 0; light_index < renderer->render.point_light_count; light_index++)
        {
            PointLight &light = renderer->render.point_lights[light_index];

            if(light_pos)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, light_pos->name, light.position);
            if(constant)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, constant->name, light.constant);
            if(linear)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, linear->name, light.linear);
            if(quadratic)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, quadratic->name, light.quadratic);
            if(ambient)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, ambient->name, light.ambient);
            if(diffuse)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, diffuse->name, light.diffuse);
            if(specular)
                rendering::set_uniform_array_value(renderer, render_command.material_handle, mapping->name, light_index, specular->name, light.specular);
        }
    }
}
