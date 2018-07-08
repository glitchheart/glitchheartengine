
static void push_particle_system(Renderer &renderer, ParticleSystemInfo &particle_info, CommandBlendMode blend_mode = CBM_ONE_MINUS_SRC_ALPHA)
{
    renderer.command_count++;
    RenderCommand* render_command = push_struct(&renderer.commands, RenderCommand);
    render_command->shader_handle = -1;
    
    render_command->type = RENDER_COMMAND_PARTICLES;
    render_command->position = particle_info.transform.position;
    //render_command->rotation = mesh_info.transform.rotation;
    
    // @Incomplete
    //render_command->particles.buffer_handle = particle_info.buffer_handle;
    //render_command->particles.material.diffuse_texture = 0; // @Incomplete
    render_command->particles.offset_buffer_handle = particle_info.offset_buffer_handle;
    render_command->particles.color_buffer_handle = particle_info.color_buffer_handle;
    render_command->particles.size_buffer_handle = particle_info.size_buffer_handle;
    render_command->particles.particle_count = particle_info.particle_count;
    render_command->particles.offsets = particle_info.offsets;
    render_command->particles.colors = particle_info.colors;
    render_command->particles.sizes = particle_info.sizes;
    render_command->particles.diffuse_texture = particle_info.attributes.texture_handle;
    render_command->particles.blend_mode = blend_mode;
}

static void push_particle_system(Renderer &renderer, i32 particle_system_handle)
{
    ParticleSystemInfo &particle_info = renderer.particle_systems[particle_system_handle];
    push_particle_system(renderer, particle_info);
}

// @Move: Move the particle system functions into an engine file instead
i32 find_unused_particle(ParticleSystemInfo &particle_system)
{    
    for(i32 i = particle_system.last_used_particle; i < particle_system.max_particles; i++)
    {
        if(particle_system.particles[i].life <= 0)
        {
            particle_system.last_used_particle = i;
            return i;
        }
    }
    
    for(int i = 0; i < particle_system.last_used_particle; i++)
    {
        if(particle_system.particles[i].life <= 0)
        {
            particle_system.last_used_particle = i;
            return i;
        }
    }
    
    return -1;
}

void update_particles(Renderer &renderer, ParticleSystemInfo &particle_system, r64 delta_time)
{
    particle_system.particle_count = 0;
    
    for(i32 i = 0; i < particle_system.max_particles; i++)
    {
        Particle& p = particle_system.particles[i];
        
        auto start = p.life > particle_system.attributes.life_time - 0.001 && p.life < particle_system.attributes.life_time + 0.001;
        
        if(p.life > 0.0f)
        {
            auto speed_value_count = particle_system.speed_over_lifetime.value_count;
            auto color_value_count = particle_system.color_over_lifetime.value_count;
            auto size_value_count = particle_system.size_over_lifetime.value_count;               
            
            if(speed_value_count > 0)
            {
                auto speed_index = p.speed_over_lifetime_index;
                
                if(speed_index < speed_value_count)
                {
                    auto start_time = particle_system.speed_over_lifetime.keys[speed_index - 1] * particle_system.attributes.life_time;
                    auto end_time = particle_system.speed_over_lifetime.keys[speed_index] * particle_system.attributes.life_time;
                    
                    auto time_spent = start_time + (particle_system.attributes.life_time - p.life);
                    
                    auto t_speed = time_spent / end_time;                      
                    
                    auto start_speed = particle_system.speed_over_lifetime.values[speed_index - 1];
                    auto end_speed = particle_system.speed_over_lifetime.values[speed_index];
                    
                    p.position += p.direction * math::lerp((r32)start_speed, (r32)t_speed, (r32)end_speed) * (r32)delta_time;
                    
                    if(time_spent >= end_time)
                    {
                        p.speed_over_lifetime_index++;
                    }
                }
                else
                {
                    p.position += p.direction * particle_system.speed_over_lifetime.values[p.speed_over_lifetime_index - 1] * (r32)delta_time;				
                }
            }
            else
            {
                p.position += p.direction * particle_system.attributes.speed_multiplier * (r32)delta_time;				
            }                
            
            if(color_value_count > 0)
            {
                auto color_index = p.color_over_lifetime_index;
                
                if(color_index < color_value_count)
                {
                    auto start_time = particle_system.color_over_lifetime.keys[color_index - 1] * particle_system.attributes.life_time;
                    auto end_time = particle_system.color_over_lifetime.keys[color_index] * particle_system.attributes.life_time;
                    
                    auto time_spent = start_time + (particle_system.attributes.life_time - p.life);
                    
                    auto t_color = time_spent / end_time;                      
                    
                    auto start_color = particle_system.color_over_lifetime.values[color_index - 1];
                    auto end_color = particle_system.color_over_lifetime.values[color_index];
                    p.color = math::lerp(start_color, (r32)t_color, end_color);
                    
                    if(time_spent >= end_time)
                    {
                        p.color_over_lifetime_index++;
                    }
                }
                else
                {
                    p.color = particle_system.color_over_lifetime.values[color_index - 1];
                }
            }
            else
            {
                p.color = particle_system.attributes.color;				
            }
            
            if(size_value_count > 0)
            {
                auto size_index = p.size_over_lifetime_index;
                
                if(size_index < size_value_count)
                {
                    auto start_time = particle_system.size_over_lifetime.keys[size_index - 1] * particle_system.attributes.life_time;
                    
                    auto end_time = particle_system.size_over_lifetime.keys[size_index] * particle_system.attributes.life_time;
                    
                    auto time_spent = start_time + (particle_system.attributes.life_time - p.life);
                    
                    auto t_size = time_spent / end_time;                      
                    
                    auto start_size = particle_system.size_over_lifetime.values[size_index - 1];
                    auto end_size = particle_system.size_over_lifetime.values[size_index];
                    p.size = math::lerp(start_size, (r32)t_size, end_size);
                    
                    if(time_spent >= end_time)
                    {
                        p.size_over_lifetime_index++;
                    }
                }
                else
                {
                    p.size = particle_system.size_over_lifetime.values[size_index - 1];
                }
            }
            else
            {
                p.size = particle_system.attributes.size;				
            }
            
            if(particle_system.attributes.particle_space == PS_WORLD && !start)
            {
                particle_system.offsets[particle_system.particle_count] = p.position + p.relative_position;
            }
            else
            {
                particle_system.offsets[particle_system.particle_count] = p.position + particle_system.transform.position;
                p.relative_position = particle_system.transform.position;
            }
            
            particle_system.colors[particle_system.particle_count] = p.color;
            particle_system.sizes[particle_system.particle_count] = p.size;
            p.life -= delta_time;
            particle_system.particle_count++;
        }
    } 
}

void merge(math::Vec3 *work_offsets, math::Vec2 *work_sizes, math::Rgba *work_colors, i32 size, i32 left, i32 mid, math::Vec3 *offsets, math::Vec2 *sizes, math::Rgba *colors, math::Vec3 camera_position)
{
    i32 right = mid + mid - left;
    if (right > size)
        right = size;
    
    i32 i = left;
    i32 j = mid;
    i32 k = left;
    
    while (i < mid && j < right)
    {
        auto i_dist = math::length(offsets[i] - camera_position);  
        auto j_dist = math::length(offsets[j] - camera_position);  
        if (i_dist > j_dist)
        {
            work_offsets[k] = offsets[i];
            work_sizes[k] = sizes[i];
            work_colors[k] = colors[i];
            k++;
            i++;
        }
        else
        {
            work_offsets[k] = offsets[j];
            work_sizes[k] = sizes[j];
            work_colors[k] = colors[j];
            k++;
            j++;
        }
        
    }
    
    while (i < mid)
    {
        work_offsets[k] = offsets[i];
        work_sizes[k] = sizes[i];
        work_colors[k] = colors[i];
        k++;
        i++;
    }
    
    while (j < right)
    {
        work_offsets[k] = offsets[j];
        work_sizes[k] = sizes[j];
        work_colors[k] = colors[j];
        k++;
        j++;
    }
    
    for (i = left; i < right; ++i)
    {
        offsets[i] = work_offsets[i];
        sizes[i] = work_sizes[i];
        colors[i] = work_colors[i];
    }
}

void sort(math::Vec3 camera_position, math::Vec3 *offsets, math::Vec2 *sizes, math::Rgba *colors, i32 n, MemoryArena *arena)
{
    i32 subsize, left, mid;
    auto temp_mem = begin_temporary_memory(arena);
    math::Vec3 *work_offsets = push_array(arena, n, math::Vec3);
    math::Vec2 *work_sizes = push_array(arena, n, math::Vec2);
    math::Rgba *work_colors = push_array(arena, n, math::Rgba);
    
    for (subsize = 1; subsize < n; subsize *= 2)
    {
        for (left = 0, mid = subsize; mid < n; left = mid + subsize, mid = left + subsize)
        {
            merge(work_offsets, work_sizes, work_colors, n, left, mid, offsets, sizes, colors, camera_position);
        }
    }
    
    end_temporary_memory(temp_mem);
}

void update_particle_systems(Renderer &renderer, r64 delta_time)
{
    for(i32 particle_system_index = 0; particle_system_index < renderer.particle_system_count; particle_system_index++)
    {
        ParticleSystemInfo &particle_system = renderer.particle_systems[particle_system_index];
        
        if (particle_system.running)
        {
            if (particle_system.emitting)
            {
                auto per_second = (r64)particle_system.attributes.particles_per_second * delta_time;
                
                i32 new_particles;
                
                if(per_second < 1.0 && per_second > 0.0)
                {
                    new_particles = 1;
                }
                else
                {
                    new_particles = (i32)per_second;
                }
                
                if (particle_system.attributes.one_shot && particle_system.total_emitted + new_particles >= particle_system.max_particles)
                {
                    new_particles = particle_system.max_particles - particle_system.total_emitted;
                    
                    particle_system.emitting = false;
                }
                
                for (i32 i = 0; i < new_particles; i++)
                {
                    i32 particle_index = find_unused_particle(particle_system);
                    if(particle_index == -1)
                    {
                        break;
                    }
                    particle_system.particles[particle_index].life = particle_system.attributes.life_time;
                    particle_system.particles[particle_index].position = math::Vec3(0, 0, 0);
                    
                    math::Vec3 random_dir = math::Vec3(
                        (rand() % 2000 - 1000.0f) / 1000.0f,
                        (rand() % 2000 - 1000.0f) / 1000.0f,
                        (rand() % 2000 - 1000.0f) / 1000.0f
                        );
                    
                    particle_system.particles[particle_index].direction = math::normalize(particle_system.attributes.direction + random_dir * particle_system.attributes.spread);
                    particle_system.particles[particle_index].color = particle_system.attributes.color;
                    particle_system.particles[particle_index].size = particle_system.attributes.size;
                    particle_system.particles[particle_index].speed_over_lifetime_index = 1;
                    particle_system.particles[particle_index].color_over_lifetime_index = 1;
                    particle_system.particles[particle_index].size_over_lifetime_index = 1;
                    
                    if(particle_system.attributes.one_shot)
                        particle_system.total_emitted++;
                }
            }
            
            update_particles(renderer, particle_system, delta_time);
            
            auto camera_position = renderer.cameras[renderer.current_camera_handle].position;
            sort(camera_position, particle_system.offsets, particle_system.sizes, particle_system.colors, particle_system.particle_count, &renderer.particle_arena);
            
            push_particle_system(renderer, particle_system);
        }   
    }
}
// @Move: END


