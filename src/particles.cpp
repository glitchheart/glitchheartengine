
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

void find_unused_particles(ParticleSystemInfo &particle_system)
{
    auto &unused_particles = particle_system.unused_particles;
    particle_system.unused_particle_count = 0;
    
    for(i32 particle_index = 0; particle_index < particle_system.max_particles / 4; particle_index++)
    {
        if(!any_nz(particle_system.particles.life[particle_index]))
        {
            unused_particles[particle_system.unused_particle_count++] = particle_index;
        }
    }
    
}

i32 find_unused_particle(ParticleSystemInfo &particle_system)
{    
    if(particle_system.unused_particle_count > 0)
    {
        return particle_system.unused_particles[particle_system.unused_particle_count-- - 1];
    }
    
    return -1;
}

S_r64 get_t(S_r64 time_spent, S_r64 start_time, S_r64 end_time)
{
    S_r64 diff = end_time - start_time;
    S_r64 in_this_index = time_spent - start_time;
    
    S_r64 index_over_diff = in_this_index / diff;
    
    S_r64 t = simd_min(1.0, index_over_diff);                      
    
    return t;
}

S_Rgba get_color_by_time(ParticleSystemInfo &particle_system, S_r64 time_spent)
{
    auto value_count = particle_system.color_over_lifetime.value_count;
    auto values = particle_system.color_over_lifetime.values;
    auto keys = particle_system.color_over_lifetime.keys;
    
    if(equal_epsilon(time_spent, particle_system.attributes.life_time, 0.001))
    {
        return S_Rgba(values[value_count - 1]);
    }
    
    for(i32 key = 0; key < value_count; key++)
    {
        if(key + 1 == value_count)
        {
            return S_Rgba(values[key]);
        }
        
        auto current_key = keys[key] * particle_system.attributes.life_time;
        auto next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(S_r64(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            S_Rgba start_color = S_Rgba(values[key]);
            S_Rgba end_color = S_Rgba(values[key + 1]);
            
            S_r64 t_color = get_t(time_spent, current_key, next_key);
            
            return math::lerp(start_color, t_color, end_color);
        }
    }
    return S_Rgba(values[value_count - 1]);
}

S_Vec2 get_size_by_time(ParticleSystemInfo &particle_system, S_r64 time_spent)
{
    auto value_count = particle_system.size_over_lifetime.value_count;
    auto values = particle_system.size_over_lifetime.values;
    auto keys = particle_system.size_over_lifetime.keys;
    
    if(equal_epsilon(time_spent, particle_system.attributes.life_time, 0.001))
    {
        return S_Vec2(values[value_count - 1]);
    }
    
    for(i32 key = 0; key < value_count; key++)
    {
        if(key + 1 == value_count)
        {
            return S_Vec2(values[key]);
        }
        
        auto current_key = keys[key] * particle_system.attributes.life_time;auto next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(S_r64(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            S_Vec2 start_size = S_Vec2(values[key]);
            S_Vec2 end_size = S_Vec2(values[key + 1]);
            
            S_r64 t_size = get_t(time_spent, current_key, next_key);
            
            return math::lerp(start_size, t_size, end_size);
        }
    }
    return S_Vec2(values[value_count - 1]);
}

S_r32 get_speed_by_time(ParticleSystemInfo &particle_system, S_r64 time_spent)
{
    auto value_count = particle_system.speed_over_lifetime.value_count;
    auto values = particle_system.speed_over_lifetime.values;
    auto keys = particle_system.speed_over_lifetime.keys;
    
    if(equal_epsilon(time_spent, particle_system.attributes.life_time, 0.001))
    {
        return S_r32(values[value_count - 1]);
    }
    
    for(i32 key = 0; key < value_count; key++)
    {
        if(key + 1 == value_count)
        {
            return S_r32(values[key]);
        }
        
        auto current_key = keys[key] * particle_system.attributes.life_time;
        auto next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(S_r64(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            S_r32 start_speed = S_r32(values[key]);
            S_r32 end_speed = S_r32(values[key + 1]);
            
            S_r64 t_speed = get_t(time_spent, current_key, next_key);
            
            return math::lerp(start_speed, t_speed, end_speed);
        }
    }
    return S_r32(values[value_count - 1]);
}

void update_particles(Renderer &renderer, ParticleSystemInfo &particle_system, r64 delta_time)
{
    particle_system.particle_count = 0;
    
    for(i32 main_index = 0; main_index < particle_system.max_particles / 4; main_index++)
    {
        auto life_non_zero = any_nz(particle_system.particles.life[main_index]);
        
        auto start = equal_epsilon(particle_system.particles.life[main_index], particle_system.attributes.life_time, 0.001);
        
        auto life = particle_system.particles.life[main_index];
        S_r64 time_spent = particle_system.attributes.life_time - life;
        
        if(life_non_zero)
        {
            auto speed_value_count = particle_system.speed_over_lifetime.value_count;
            auto color_value_count = particle_system.color_over_lifetime.value_count;
            auto size_value_count = particle_system.size_over_lifetime.value_count;               
            
            if(size_value_count > 0)
            {
                particle_system.particles.size[main_index] = get_size_by_time(particle_system, time_spent);
            }
            
            if(color_value_count > 0)
            {
                particle_system.particles.color[main_index] = get_color_by_time(particle_system, time_spent);
            }
            else
            {
                particle_system.particles.color[main_index] = particle_system.attributes.start_color;
            }
            
            if(speed_value_count > 0)
            {
                particle_system.particles.position[main_index] += particle_system.particles.direction[main_index] * get_speed_by_time(particle_system, time_spent) * (r32)delta_time;
            }
            else
            {
                particle_system.particles.position[main_index] += particle_system.particles.direction[main_index] * particle_system.attributes.start_speed * (r32)delta_time;				
            }                
            
            for(i32 sub_index = 0; sub_index < 4; sub_index++)
            {
                auto cur_pos = to_vec3(particle_system.particles.position[main_index], sub_index);
                auto relative_pos = to_vec3(particle_system.particles.relative_position[main_index], sub_index);
                
                if(particle_system.attributes.particle_space == PS_WORLD && !start)
                {
                    particle_system.offsets[particle_system.particle_count] = cur_pos + relative_pos;
                }
                else
                {
                    particle_system.offsets[particle_system.particle_count] = cur_pos + particle_system.transform.position;
                    particle_system.particles.relative_position[main_index] = particle_system.transform.position;
                }
                
                auto cur_col = to_vec4(particle_system.particles.color[main_index], sub_index);
                
                particle_system.colors[particle_system.particle_count] = cur_col;
                particle_system.sizes[particle_system.particle_count] = to_vec2(particle_system.particles.size[main_index], sub_index);
                particle_system.particle_count++;
                
            }
            
            particle_system.particles.life[main_index] -= delta_time;
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

void emit_particle(ParticleSystemInfo &particle_system)
{
    i32 original_index = find_unused_particle(particle_system);
    
    if(original_index == -1)
    {
        return;
    }
    
    particle_system.particles.life[original_index] = particle_system.attributes.life_time;
    particle_system.particles.size[original_index] = particle_system.attributes.start_size;
    particle_system.particles.color[original_index] = particle_system.attributes.start_color;
    particle_system.particles.position[original_index] = S_Vec3(0.0f);
    
    auto random_dir = random_direction();
    
    auto new_direction = S_Vec3(math::normalize(to_vec3(random_dir, 0)), math::normalize(to_vec3(random_dir, 1)), math::normalize(to_vec3(random_dir, 2)), math::normalize(to_vec3(random_dir, 3)));
    
    particle_system.particles.direction[original_index] = math::normalize(particle_system.attributes.direction + new_direction * particle_system.attributes.spread);
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
                find_unused_particles(particle_system);
                i32 new_particles;
                
                i32 burst_particles = 0;
                
                auto value_count = particle_system.attributes.emission_module.burst_over_lifetime.value_count;
                if(value_count > 0)
                {
                    auto burst_index  = particle_system.attributes.emission_module.burst_over_lifetime.current_index;
                    
                    if(burst_index < value_count)
                    {
                        auto &current_burst = particle_system.attributes.emission_module.burst_over_lifetime.values[burst_index];
                        
                        auto target_time = current_burst.repeat_interval;
                        
                        particle_system.current_emission_time += delta_time;
                        
                        if(particle_system.current_emission_time >= target_time)
                        {
                            burst_particles = current_burst.count;
                            
                            if(current_burst.cycle_count > 0 && particle_system.attributes.emission_module.burst_over_lifetime.current_index >= current_burst.cycle_count)
                            {
                                particle_system.attributes.emission_module.burst_over_lifetime.current_index++;
                            }
                            
                            particle_system.current_emission_time = 0.0;
                        }
                    }
                }
                
                auto per_second = (r64)particle_system.attributes.particles_per_second * delta_time;
                
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
                
                new_particles = math::multiple_of_number(new_particles, 4);
                burst_particles = math::multiple_of_number(burst_particles, 4);
                
                for (i32 i = 0; i < new_particles / 4; i++)
                {
                    emit_particle(particle_system);
                    
                    if(particle_system.attributes.one_shot)
                        particle_system.total_emitted += 4;
                }
                
                for (i32 i = 0; i < burst_particles / 4; i++)
                {
                    emit_particle(particle_system);
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


