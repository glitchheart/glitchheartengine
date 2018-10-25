static void push_particle_system(Renderer &renderer, ParticleSystemInfo &particle_info, CommandBlendMode blend_mode = CBM_ONE_MINUS_SRC_ALPHA)
{
    renderer.command_count++;
    RenderCommand* render_command = &renderer.commands[renderer.command_count++];
    render_command->shader_handle = -1;
    
    render_command->type = RENDER_COMMAND_PARTICLES;
    render_command->position = particle_info.transform.position;
    
    // @Incomplete:(Niels): 
    //render_command->rotation = mesh_info.transform.rotation;
    
    // @Incomplete
    //render_command->particles.buffer_handle = particle_info.buffer_handle;
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

static void push_particle_system(Renderer &renderer, ParticleSystemHandle handle)
{
    i32 _internal_handle = renderer.particles._internal_handles[handle.handle];
    
    if(_internal_handle != -1)
    {
        ParticleSystemInfo &particle_info = renderer.particles.particle_systems[_internal_handle];
        push_particle_system(renderer, particle_info);
    }
}

static void push_particle_system(Renderer &renderer, i32 particle_system_handle)
{
    i32 _internal_handle = renderer.particles._internal_handles[particle_system_handle];
    
    if(_internal_handle != -1)
    {
        ParticleSystemInfo &particle_info = renderer.particles.particle_systems[_internal_handle];
        push_particle_system(renderer, particle_info);
    }
}

i32 find_unused_particle(ParticleSystemInfo &particle_system)
{   
    if(particle_system.dead_particle_count > 0)
    {
        return particle_system.dead_particles[particle_system.dead_particle_count-- - 1];
    }
    
	assert(false);
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

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
S_Rgba get_color_by_time(ParticleSystemInfo &particle_system, S_r64 time_spent)
{
    i32 value_count = particle_system.color_over_lifetime.value_count;
    math::Rgba* values = particle_system.color_over_lifetime.values;
    r64* keys = particle_system.color_over_lifetime.keys;
    
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
        
        r64 current_key = keys[key] * particle_system.attributes.life_time;
        r64 next_key = keys[key + 1] * particle_system.attributes.life_time;
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

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
S_Vec2 get_size_by_time(ParticleSystemInfo &particle_system, S_r64 time_spent)
{
    i32 value_count = particle_system.size_over_lifetime.value_count;
    math::Vec2* values = particle_system.size_over_lifetime.values;
    r64* keys = particle_system.size_over_lifetime.keys;
    
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
        
        r64 current_key = keys[key] * particle_system.attributes.life_time;r64  next_key = keys[key + 1] * particle_system.attributes.life_time;
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

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
S_r32 get_speed_by_time(ParticleSystemInfo &particle_system, S_r64 time_spent)
{
    i32 value_count = particle_system.speed_over_lifetime.value_count;
    r32* values = particle_system.speed_over_lifetime.values;
    r64* keys = particle_system.speed_over_lifetime.keys;
    
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
        
        r64 current_key = keys[key] * particle_system.attributes.life_time;
        r64 next_key = keys[key + 1] * particle_system.attributes.life_time;
        if(any_lt_eq(S_r64(current_key), time_spent) && any_lt_eq(time_spent, next_key))
        {
            S_r32 start_speed = S_r32(values[key]);
            S_r32 end_speed = S_r32(values[key + 1]);
            
            S_r64 t_speed = get_t(time_spent, current_key, next_key);
            
            S_r32 res = math::lerp(start_speed, t_speed, end_speed);
            
            return res;
        }
    }
    
    return S_r32(values[value_count - 1]);
}

// @Note:(Niels): Update the particles that have been emitted in previous and in the current frame
// Kills particles in case they have life <= 0.0 and adds them to the dead particle buffer
// Dead particles are used when emitting in the emit procedure.
// Any particle that is NOT dead, will be written into the alive buffer of the next frame
// This ensures that we can clear the current frame buffer, and only ever iterate over
// particles that are actually alive.
void update_particles(Renderer &renderer, ParticleSystemInfo &particle_system, r64 delta_time, i32 *emitted_buf, i32 *emitted_this_frame, i32* next_frame_buf, i32 *next_frame_count)
{
    particle_system.particle_count = 0;
    
    i32 speed_value_count = particle_system.speed_over_lifetime.value_count;
    i32 color_value_count = particle_system.color_over_lifetime.value_count;
    i32 size_value_count = particle_system.size_over_lifetime.value_count;               
    
    for(i32 alive_index = 0; alive_index < *emitted_this_frame; alive_index++)
    {
        i32 main_index = emitted_buf[alive_index];
        
        // @Incomplete:(Niels): Used to check where to position initial emission
        // Maybe find a better solution that doesn't require branching on every particle?
        auto start = equal_epsilon(particle_system.particles.life[main_index], particle_system.attributes.life_time, 0.001);
        
        particle_system.particles.life[main_index] -= delta_time;
        
        // @Note:(Niels): Check for alive state of this particle and kill if dead.
        // Seems like a necessary branch here, since we do need to know if a particle is dead.
        // Maybe there is some SIMD magic, that can do this for us (probably not...).
        if(any_lt_eq(particle_system.particles.life[main_index], 0.0))
        {
            particle_system.dead_particles[particle_system.dead_particle_count++] = main_index;
            continue;
        }
        else
        {
            next_frame_buf[(*next_frame_count)++] = main_index;
        }
        
        auto life = particle_system.particles.life[main_index];
        S_r64 time_spent = particle_system.attributes.life_time - life;
        
        particle_system.particles.direction[main_index] += S_Vec3(math::Vec3(0.0f, -particle_system.attributes.gravity * (r32)delta_time, 0.0f));
        
        
        // @Note(Niels): This branch will always be true or false for the whole loop so it should be optimized out
        if(size_value_count > 0)
        {
            particle_system.particles.size[main_index] = get_size_by_time(particle_system, time_spent);
        }
        else
        {
            particle_system.particles.size[main_index] = particle_system.attributes.start_size;
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
        
        S_Vec3 final_pos(0.0f);
        
        if(particle_system.attributes.particle_space == PS_WORLD && !start)
        {
            final_pos = particle_system.particles.position[main_index] + particle_system.particles.relative_position[main_index];
        }
        else
        {
            final_pos = particle_system.particles.position[main_index] + particle_system.transform.position;
            particle_system.particles.relative_position[main_index] = particle_system.transform.position;
        }
        
        S_Rgba color = particle_system.particles.color[main_index];
        S_Vec2 size = particle_system.particles.size[main_index];
        
        float p1[4], p2[4], p3[4], p4[4];
        float s1[4], s2[4], s3[4], s4[4];
        float c1[4], c2[4], c3[4], c4[4];
        
        s_vec2_to_float4(size, s1, s2, s3, s4);
        s_vec3_to_float4(final_pos, p1, p2, p3, p4);
        s_vec4_to_float4(color, c1, c2, c3, c4);
        
        particle_system.offsets[particle_system.particle_count].x = p1[0];
        particle_system.offsets[particle_system.particle_count].y = p1[1];
        particle_system.offsets[particle_system.particle_count].z = p1[2];
        
        particle_system.colors[particle_system.particle_count].r = c1[0];
        particle_system.colors[particle_system.particle_count].g = c1[1];
        particle_system.colors[particle_system.particle_count].b = c1[2];
        particle_system.colors[particle_system.particle_count].a = c1[3];
        
        particle_system.sizes[particle_system.particle_count].x = s1[0];
        particle_system.sizes[particle_system.particle_count].y = s1[1];
        
        particle_system.particle_count++;
        
        particle_system.offsets[particle_system.particle_count].x = p2[0];
        particle_system.offsets[particle_system.particle_count].y = p2[1];
        particle_system.offsets[particle_system.particle_count].z = p2[2];
        
        particle_system.colors[particle_system.particle_count].r = c2[0];
        particle_system.colors[particle_system.particle_count].g = c2[1];
        particle_system.colors[particle_system.particle_count].b = c2[2];
        particle_system.colors[particle_system.particle_count].a = c2[3];
        
        particle_system.sizes[particle_system.particle_count].x = s2[0];
        particle_system.sizes[particle_system.particle_count].y = s2[1];
        
        particle_system.particle_count++;
        
        particle_system.offsets[particle_system.particle_count].x = p3[0];
        particle_system.offsets[particle_system.particle_count].y = p3[1];
        particle_system.offsets[particle_system.particle_count].z = p3[2];
        
        particle_system.colors[particle_system.particle_count].r = c3[0];
        particle_system.colors[particle_system.particle_count].g = c3[1];
        particle_system.colors[particle_system.particle_count].b = c3[2];
        particle_system.colors[particle_system.particle_count].a = c3[3];
        
        particle_system.sizes[particle_system.particle_count].x = s3[0];
        particle_system.sizes[particle_system.particle_count].y = s3[1];
        
        particle_system.particle_count++;
        
        particle_system.offsets[particle_system.particle_count].x = p4[0];
        particle_system.offsets[particle_system.particle_count].y = p4[1];
        particle_system.offsets[particle_system.particle_count].z = p4[2];
        
        particle_system.colors[particle_system.particle_count].r = c4[0];
        particle_system.colors[particle_system.particle_count].g = c4[1];
        particle_system.colors[particle_system.particle_count].b = c4[2];
        particle_system.colors[particle_system.particle_count].a = c4[3];
        
        particle_system.sizes[particle_system.particle_count].x = s4[0];
        particle_system.sizes[particle_system.particle_count].y = s4[1];
        
        particle_system.particle_count++;
    }
    
    *emitted_this_frame = 0;
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

void emit_particle(ParticleSystemInfo &particle_system, i32* alive_buf, i32* count)
{
    i32 original_index = find_unused_particle(particle_system);
    
    assert(original_index != -1);
    
    particle_system.particles.life[original_index] = particle_system.attributes.life_time;
    particle_system.particles.size[original_index] = particle_system.attributes.start_size;
    particle_system.particles.color[original_index] = particle_system.attributes.start_color;
    
    S_Vec3 random_dir;
    S_Vec3 new_direction;
    
    assert(particle_system.attributes.emission_module.emitter_func);
    
    ParticleSpawnInfo spawn_info = particle_system.attributes.emission_module.emitter_func(particle_system.entropy);
    particle_system.particles.position[original_index] = spawn_info.position;
    new_direction = spawn_info.direction;
    
    particle_system.particles.direction[original_index] = math::normalize((particle_system.attributes.direction + new_direction) * particle_system.attributes.spread);
    
    // @Note:(Niels): The current buffer gets the particle being emitted
    // It is passed in, so we only need to check once per frame when the system is updated
    alive_buf[(*count)++] = original_index;
    
    // @Note:(Niels): 4 == SIMD? Maybe revisit how we think about particles always in 4's with SIMD...
    // This seems somewhat dumb.
    particle_system.particles_emitted_this_frame += 4;
}

void update_particle_systems(Renderer &renderer, r64 delta_time)
{
    for(i32 particle_system_index = 0; particle_system_index < renderer.particles.particle_system_count; particle_system_index++)
    {
        ParticleSystemInfo &particle_system = renderer.particles.particle_systems[particle_system_index];
        
        if (particle_system.running)
        {
            i32* emitted_alive_buf = nullptr;
            i32* emitted_alive_count = nullptr;
            i32* write_buf = nullptr;
            i32* write_buf_count = nullptr;
            if(particle_system.alive0_active)
            {
                emitted_alive_buf = particle_system.alive0_particles;
                emitted_alive_count = &particle_system.alive0_particle_count;
                write_buf = particle_system.alive1_particles;
                write_buf_count = &particle_system.alive1_particle_count;
            }
            else
            {
                emitted_alive_buf = particle_system.alive1_particles;
                emitted_alive_count = &particle_system.alive1_particle_count;
                write_buf = particle_system.alive0_particles;
                write_buf_count = &particle_system.alive0_particle_count;
            }
            
            particle_system.alive0_active = !particle_system.alive0_active;
            
            if (particle_system.emitting)
            {
                particle_system.time_spent += delta_time;
                //find_unused_particles(particle_system);
                
                i32 new_particles;
                
                // @Note(Niels): Figure out the burst amount if there is any
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
                
                // @Note(Niels): Start figuring out how many particles we need to emit this frame
                r32 per_second = (r32)((r64)particle_system.attributes.particles_per_second * delta_time);
                
                // @Note(Niels): We need to check if we've spent 1 second if per second is lower than 1.0
                if(per_second < 1.0f && per_second > 0.0f && particle_system.time_spent >= 1.0)
                {
                    particle_system.time_spent = 0.0;
                    new_particles = 1;
                }
                else
                {
                    new_particles = math::round(per_second);
                }
                
                if (particle_system.attributes.one_shot && particle_system.total_emitted + new_particles >= particle_system.max_particles)
                {
                    new_particles = particle_system.max_particles - particle_system.total_emitted;
                    
                    particle_system.emitting = false;
                }
                
                // @Incomplete:(Niels): Consider if it is even encessary to have these simd values??
                // seems kind of dumb...
                i32 simd_new_particles = math::multiple_of_number(new_particles, 4);
                i32 simd_burst_particles = math::multiple_of_number(burst_particles, 4);
                
                simd_new_particles = MIN(simd_new_particles, particle_system.dead_particle_count);
                
                // @Note(Niels): Emit the particles into the current alive buffer
                // The first time around this buffer is empty, but on any subsequent step
                // there should already be particles from the previous frame, which were
                // written into the then next buffer (which is now the alive buffer)
                for (i32 i = 0; i < simd_new_particles / 4; i++)
                {
                    emit_particle(particle_system, emitted_alive_buf, emitted_alive_count);
                }
                
                // @Note(Niels): Same goes for burst
                if(particle_system.attributes.one_shot)
                    particle_system.total_emitted += new_particles;
                
                for (i32 i = 0; i < simd_burst_particles / 4; i++)
                {
                    emit_particle(particle_system, emitted_alive_buf, emitted_alive_count);
                }
                
                debug("emitted: %d\n", particle_system.total_emitted);
            }
            
            // @Note:(Niels): We now emit the particles in the emitted alive buf (which may contain particles from previous frames that are still alive), while passing in the next buffer,
            // which is now our "write" buffer.
            update_particles(renderer, particle_system, delta_time, emitted_alive_buf, emitted_alive_count, write_buf, write_buf_count);
            
            
            // if all particles are dead and the system is one-shot we should stop the particle_system
            if(particle_system.total_emitted == particle_system.max_particles && particle_system.attributes.one_shot)
            {
                particle_system.emitting = false;
            }
            
            //auto camera_position = renderer.cameras[renderer.current_camera_handle].position;
            //sort(camera_position, particle_system.offsets, particle_system.sizes, particle_system.colors, particle_system.particle_count, &renderer.particle_arena);
            
            push_particle_system(renderer, particle_system);
        }   
    }
}
// @Move: END


