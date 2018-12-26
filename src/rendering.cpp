#include "animation.h"
#include <string.h>

// The InfoHandle is used to be able to reference the same animation without having to load the animation again. 
static void add_animation(Renderer* renderer, SpritesheetAnimation animation, const char* animation_name)
{
    strcpy(animation.name, animation_name);
    renderer->spritesheet_animations[renderer->spritesheet_animation_count++] = animation;
}

static void set_new_resolution(Renderer *renderer, i32 new_width, i32 new_height)
{
    renderer->window_width = new_width;
    renderer->window_height = new_height;
}

static void set_new_resolution(Renderer *renderer, Resolution new_resolution)
{
    set_new_resolution(renderer, new_resolution.width, new_resolution.height);
}

static void set_new_resolution(Renderer *renderer, i32 resolution_index)
{
    auto new_resolution = renderer->available_resolutions[resolution_index];
    renderer->current_resolution_index = resolution_index;
    set_new_resolution(renderer, new_resolution.width, new_resolution.height);
}

static void set_new_window_mode(Renderer *renderer, WindowMode new_window_mode)
{
    renderer->window_mode = new_window_mode;
}

static void update_lighting_for_material(BatchedCommand &render_command, Renderer *renderer)
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

#define STANDARD_PASS_HANDLE { 1 }

static void push_scene_for_rendering(scene::Scene &scene, Renderer *renderer)
{
    renderer->camera = scene.camera;

    for(i32 i = 0; i < renderer->render.pass_count; i++)
    {
        rendering::RenderPass &pass = renderer->render.passes[i];
        if(pass.use_scene_camera)
        {
            pass.camera = scene.camera;
        }
    }
    
    renderer->render.dir_light_count = 0;
    renderer->render.point_light_count = 0;
    
    QueuedRenderCommand *queued_commands = renderer->render.queued_commands;
    i32 normal_count = 0;

    i32 particles_to_push[64];
    i32 particles_count = 0;
    
    for(i32 ent_index = 0; ent_index < scene.entity_count; ent_index++)
    {
        const scene::Entity &ent = scene.entities[ent_index];
        
        if (scene.active_entities[ent_index])
        {
            scene::TransformComponent &transform = scene.transform_components[ent.transform_handle.handle];
            
            // Create a copy of the position, rotation and scale since we don't want the parents transform to change the child's transform. Only when rendering.
            math::Vec3 position = transform.position;
            math::Vec3 rotation = transform.rotation;
            math::Vec3 scale = transform.scale;
            
            if(ent.comp_flags & scene::COMP_LIGHT)
            {
                scene::LightComponent &light_comp = scene.light_components[ent.light_handle.handle];
                
                switch(light_comp.type)
                {
                case scene::LightType::DIRECTIONAL:
                renderer->render.directional_lights[renderer->render.dir_light_count++] = light_comp.dir_light;
                break;
                case scene::LightType::POINT:
                light_comp.point_light.position = position;
                renderer->render.point_lights[renderer->render.point_light_count++] = light_comp.point_light;
                break;
                default:
                assert(false);
                }
            }

            if (ent.comp_flags & scene::COMP_RENDER)
            {
                scene::RenderComponent &render = scene.render_components[ent.render_handle.handle];
                
                if(render.is_new_version)
                {
                    QueuedRenderCommand *command = nullptr;
                    rendering::Material &instance = get_material_instance(render.v2.material_handle, renderer);
                    
                    for(i32 i = 0; i < normal_count; i++)
                    {
                        QueuedRenderCommand &cmd = queued_commands[i];
                        if(cmd.buffer_handle.handle == render.v2.buffer_handle.handle
                            && cmd.original_material.handle == instance.source_material.handle)
                        {
                            // It's a doozy
                            command = &cmd;
                            break;
                        }
                    }

                    if(!command)
                    {
                        command = &queued_commands[normal_count++];
                        command->buffer_handle = render.v2.buffer_handle;
                        command->original_material = instance.source_material;
                        command->count = 0;
                    }
                    
                    rendering::Transform t;
                    t.position = position;
                    t.rotation = rotation;
                    t.scale = scale;

                    if(render.v2.render_pass_count > 0)
                    {
                        BatchedCommand &batch_command = command->commands[command->count];
                        batch_command.transform = t;
                        batch_command.material_handle = render.v2.material_handle;
                        batch_command.casts_shadows = render.casts_shadows;
                        batch_command.pass_count = render.v2.render_pass_count;
                        
                        for(i32 i = 0; i < render.v2.render_pass_count; i++)
                        {
                            batch_command.passes[i] = render.v2.render_passes[i];
                            batch_command.shader_handles[i] = render.v2.shader_handles[i];
                        }
                        
                        command->count++;
                    }
                }
            }
            else if(ent.comp_flags & scene::COMP_PARTICLES)
            {
                if(ent.particle_system_handle.handle != -1)
                {
                    scene::ParticleSystemComponent &ps = scene.particle_system_components[ent.particle_system_handle.handle];
                    
                    i32 _internal_handle = renderer->particles._internal_handles[ps.handle.handle - 1];
                    ParticleSystemInfo& system = renderer->particles.particle_systems[_internal_handle];
                    
                    system.transform.position = system.attributes.base_position;
                    
                    if(ent.comp_flags & scene::COMP_TRANSFORM)
                    {
                        system.transform.position += position;
                        system.transform.scale = scale;
                        system.transform.rotation = rotation;
                    }

                    if(system.running)
                    {
                        particles_to_push[particles_count++] = ps.handle.handle;
                    }
                }
            }
        }
    }

    for(i32 index = 0; index < normal_count; index++)
    {
        QueuedRenderCommand &queued_command = queued_commands[index];
        
        BatchedCommand &first_command = queued_command.commands[0];
        rendering::Material &material = get_material_instance(first_command.material_handle, renderer);

        if(material.lighting.receives_light)
            update_lighting_for_material(first_command, renderer);

        for(i32 batch_index = 0; batch_index < queued_command.count; batch_index++)
        {
            BatchedCommand &render_command = queued_command.commands[batch_index];

            rendering::Material &mat_instance = get_material_instance(render_command.material_handle, renderer);

            if (mat_instance.instanced_vertex_attribute_count == 0)
            {
                // Just push the buffer as a normal draw call
                for (i32 pass_index = 0; pass_index < render_command.pass_count; pass_index++)
                {
                    rendering::push_buffer_to_render_pass(renderer, queued_command.buffer_handle, first_command.material_handle, render_command.transform, render_command.shader_handles[pass_index], render_command.passes[pass_index]);
                }
                continue;
            }
            else
            {
                // We can make one call instead
                for (i32 i = 0; i < mat_instance.instanced_vertex_attribute_count; i++)
                {
                    rendering::VertexAttributeInstanced &va = mat_instance.instanced_vertex_attributes[i];
                    rendering::VertexAttribute &attr = va.attribute;
                        
                    switch (attr.type)
                    {
                    case rendering::ValueType::FLOAT:
                    rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float_val, renderer);
                    break;
                    case rendering::ValueType::FLOAT2:
                    rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float2_val, renderer);
                    break;
                    case rendering::ValueType::FLOAT3:
                    {
                        math::Vec3 val = attr.float3_val;
                            
                        if(va.mapping_type != rendering::VertexAttributeMappingType::NONE)
                        {
                            if(va.mapping_type == rendering::VertexAttributeMappingType::POSITION)
                                val = render_command.transform.position;
                            else if(va.mapping_type == rendering::VertexAttributeMappingType::ROTATION)
                                val = render_command.transform.rotation;
                            else if(va.mapping_type == rendering::VertexAttributeMappingType::SCALE)
                                val = render_command.transform.scale;
                        }
                        rendering::add_instance_buffer_value(va.instance_buffer_handle, val, renderer);
                    }
                    break;
                    case rendering::ValueType::FLOAT4:
                    rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float4_val, renderer);
                    break;
                    case rendering::ValueType::MAT4:
                    {
                        math::Mat4 val = attr.mat4_val;
                            
                        if(va.mapping_type == rendering::VertexAttributeMappingType::MODEL)
                        {
                            rendering::Transform &transform = render_command.transform;
                                
                            math::Mat4 model_matrix(1.0f);
                            model_matrix = math::scale(model_matrix, transform.scale);
    
                            math::Vec3 rotation = transform.rotation;
                            auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
                            auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
                            auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;
    
                            math::Quat orientation = math::Quat();
                            orientation = math::rotate(orientation, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
                            orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
                            orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));
    
                            model_matrix = to_matrix(orientation) * model_matrix;
    
                            model_matrix = math::translate(model_matrix, transform.position);
                            val = model_matrix;
                        }
                        rendering::add_instance_buffer_value(va.instance_buffer_handle, val, renderer);
                    }
                    break;
                    default:
                    assert(false);
                    }
                }
            }
		}
        
        // Push the command to the shadow buffer if it casts shadows
        if(first_command.casts_shadows)
        {
            rendering::push_instanced_buffer_to_shadow_pass(renderer, queued_command.count, queued_command.buffer_handle, material.instanced_vertex_attributes, material.instanced_vertex_attribute_count);
        }
        
        // Push the command to the correct render passes
        for (i32 pass_index = 0; pass_index < first_command.pass_count; pass_index++)
        {
            rendering::push_instanced_buffer_to_render_pass(renderer, queued_command.count, queued_command.buffer_handle, first_command.material_handle, first_command.shader_handles[pass_index], first_command.passes[pass_index]);
        }
    }
    
    for(i32 i = 0; i < particles_count; i++)
    {
        i32 _internal_handle = renderer->particles._internal_handles[particles_to_push[i] - 1];
        ParticleSystemInfo& system = renderer->particles.particle_systems[_internal_handle];
        rendering::Material& particle_material = get_material_instance(system.material_handle, renderer);
            
        rendering::push_instanced_buffer_to_render_pass(renderer, system.particle_count, renderer->particles.quad_buffer, system.material_handle, particle_material.shader, rendering::get_render_pass_handle_for_name(STANDARD_PASS, renderer));
    }    
}
