void update_particle_systems(Renderer *renderer, r64 delta_time);
void update_particle_system(ParticleSystemInfo& particle_system, Renderer *renderer, r64 delta_time);
void emit_particle(Renderer *renderer, ParticleSystemInfo &particle_system, i32* alive_buf, i32* count, RandomSeries& entropy, i32 emitted_count);
void update_particles(Renderer *renderer, ParticleSystemInfo &particle_system, r64 delta_time, i32 *emitted_buf, i32 *emitted_this_frame, i32* next_frame_buf, i32 *next_frame_count);
static void update_particles_job(WorkQueue *work_queue, void* data_ptr);
static void update_particle_system_job(WorkQueue *work_queue, void *data_ptr);

EMITTER_FUNC(emit_dir)
{
    ParticleSpawnInfo info;
    
    info.position = Vec3_4x(0.0f);
    info.direction = Vec3_4x(0.0f);
    
    return info;
}

EMITTER_FUNC(emit_random_dir)
{
    ParticleSpawnInfo info;
    
    info.position = Vec3_4x(0.0f);
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_2D_square)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_rect_4x(series, min, max);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_2D_square_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_rect_4x(series, min, max);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_square)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_outer_rect_4x(series, min, max, min, max);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_square_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_outer_rect_4x(series, min, max, min, max);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_disc)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_disc_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_circle)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_circle_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = Vec3_4x(0.0f, 1.0f, 0.0f);
    
    return info;
}

EMITTER_FUNC(emit_from_disc_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_disc_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}

EMITTER_FUNC(emit_from_circle_random)
{
    ParticleSpawnInfo info;
    
    Vec3_4x r = random_circle_4x(series, (max - min) / 2.0f);
    
    info.position = r;
    
    info.direction = random_direction_4x(series);
    
    return info;
}


i32 find_unused_particle(ParticleSystemInfo &particle_system)
{
	if (particle_system.dead_particle_count > 0)
	{
		particle_system.dead_particle_count--;
		return particle_system.dead_particles[particle_system.dead_particle_count];
	}

	return -1;
}

r32_4x get_t(r32_4x time_spent, r32_4x start_time, r32_4x end_time)
{
	r32_4x diff = end_time - start_time;
	r32_4x in_this_index = time_spent - start_time;

	r32_4x index_over_diff = in_this_index / diff;

	r32_4x t = simd_max(0.0, simd_min(1.0, index_over_diff));

	return t;
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
Rgba_4x get_color_by_time(ParticleSystemInfo &particle_system, i32 index, r32_4x time_spent, i32* active_indices, i32 count)
{
	i32 value_count = particle_system.color_over_lifetime.value_count;
	math::Rgba* values = particle_system.color_over_lifetime.values;
	r32* keys = particle_system.color_over_lifetime.keys;

	r32_4x start_life = particle_system.particles.start_life[index];

	Rgba_4x result = values[value_count - 1];

	i32 found_keys[4] = {};
	i32 found_next_keys[4] = {};

	for (i32 j = 0; j < count; j++)
	{
        i32 i = active_indices[j];
		for (i32 key = 0; key < value_count; key++)
		{
			r64 current_key = keys[key] * start_life.e[i];
			r64 next_key = keys[key + 1] * start_life.e[i];

			if (current_key <= time_spent.e[i] && time_spent.e[i] < next_key)
			{
				found_keys[i] = key;
				found_next_keys[i] = key + 1;
				break;
			}
			else if (key + 1 == value_count - 1 && time_spent.e[i] >= next_key)
			{
				found_keys[i] = key + 1;
				found_next_keys[i] = key + 1;
				break;
			}
		}
	}

	r32_4x current_keys = r32_4x(keys[found_keys[0]], keys[found_keys[1]], keys[found_keys[2]], keys[found_keys[3]]) * start_life;
	r32_4x next_keys = r32_4x(keys[found_next_keys[0]], keys[found_next_keys[1]], keys[found_next_keys[2]], keys[found_next_keys[3]]) * start_life;

	Rgba_4x start_color = Rgba_4x(values[found_keys[0]], values[found_keys[1]], values[found_keys[2]], values[found_keys[3]]);
	Rgba_4x end_color = Rgba_4x(values[found_next_keys[0]], values[found_next_keys[1]], values[found_next_keys[2]], values[found_next_keys[3]]);

	r32_4x t = get_t(time_spent, current_keys, next_keys);

	result = math::lerp(start_color, t, end_color);

	return result;
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
Vec2_4x get_size_by_time(ParticleSystemInfo &particle_system, i32 index, r32_4x time_spent, i32* active_indices, i32 count)
{
	i32 value_count = particle_system.size_over_lifetime.value_count;
	math::Vec2* values = particle_system.size_over_lifetime.values;
	r32* keys = particle_system.size_over_lifetime.keys;

	r32_4x start_life = particle_system.particles.start_life[index];
	Vec2_4x _start_size = particle_system.particles.start_size[index];

	Vec2_4x result = _start_size * values[value_count - 1];

	i32 found_keys[4] = {};
	i32 found_next_keys[4] = {};

	for (i32 j = 0; j < count; j++)
	{
        i32 i = active_indices[j];
		for (i32 key = 0; key < value_count; key++)
		{
			r64 current_key = keys[key] * start_life.e[i];
			r64 next_key = keys[key + 1] * start_life.e[i];

			if (current_key <= time_spent.e[i] && time_spent.e[i] < next_key)
			{
				found_keys[i] = key;
				found_next_keys[i] = key + 1;
				break;
			}
			else if (key + 1 == value_count - 1 && time_spent.e[i] >= next_key)
			{
				found_keys[i] = key + 1;
				found_next_keys[i] = key + 1;
				break;
			}
		}
	}

	r32_4x current_keys = r32_4x(keys[found_keys[0]], keys[found_keys[1]], keys[found_keys[2]], keys[found_keys[3]]) * start_life;
	r32_4x next_keys = r32_4x(keys[found_next_keys[0]], keys[found_next_keys[1]], keys[found_next_keys[2]], keys[found_next_keys[3]]) * start_life;

	Vec2_4x start_size = Vec2_4x(values[found_keys[0]], values[found_keys[1]], values[found_keys[2]], values[found_keys[3]]) * _start_size;
	Vec2_4x end_size = Vec2_4x(values[found_next_keys[0]], values[found_next_keys[1]], values[found_next_keys[2]], values[found_next_keys[3]]) * start_size;

	r32_4x t = get_t(time_spent, current_keys, next_keys);

	result = math::lerp(start_size, t, end_size);

	return result;
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
r32_4x get_speed_by_time(ParticleSystemInfo &particle_system, i32 index, r32_4x time_spent, i32* active_indices, i32 count)
{
	i32 value_count = particle_system.speed_over_lifetime.value_count;
	r32* values = particle_system.speed_over_lifetime.values;
	r32* keys = particle_system.speed_over_lifetime.keys;

	r32_4x start_life = particle_system.particles.start_life[index];
	r32_4x _start_speed = particle_system.particles.start_speed[index];

	r32_4x result = r32_4x(values[value_count - 1]);

	i32 found_keys[4] = {};
	i32 found_next_keys[4] = {};

	for (i32 j = 0; j < count; j++)
	{
        i32 i = active_indices[j];
		for (i32 key = 0; key < value_count; key++)
		{
			r64 current_key = keys[key] * start_life.e[i];
			r64 next_key = keys[key + 1] * start_life.e[i];

			if (current_key <= time_spent.e[i] && time_spent.e[i] < next_key)
			{
				found_keys[i] = key;
				found_next_keys[i] = key + 1;
				break;
			}
			else if (key + 1 == value_count - 1 && time_spent.e[i] >= next_key)
			{
				found_keys[i] = key + 1;
				found_next_keys[i] = key + 1;
				break;
			}
		}
	}

	r32_4x current_keys = r32_4x(keys[found_keys[0]], keys[found_keys[1]], keys[found_keys[2]], keys[found_keys[3]]) * start_life;
	r32_4x next_keys = r32_4x(keys[found_next_keys[0]], keys[found_next_keys[1]], keys[found_next_keys[2]], keys[found_next_keys[3]]) * start_life;

	r32_4x start_speed = r32_4x(values[found_keys[0]], values[found_keys[1]], values[found_keys[2]], values[found_keys[3]]);
	r32_4x end_speed = r32_4x(values[found_next_keys[0]], values[found_next_keys[1]], values[found_next_keys[2]], values[found_next_keys[3]]);

	r32_4x t = get_t(time_spent, current_keys, next_keys);

	result = math::lerp(start_speed, t, end_speed);

	result *= _start_speed;

	return result;
}

// @Incomplete(Niels): Rethink how this is done to be more SIMD like...
r32_4x get_angle_by_time(ParticleSystemInfo &particle_system, i32 index, r32_4x time_spent, i32* active_indices, i32 count)
{
	i32 value_count = particle_system.angle_over_lifetime.value_count;
	r32* values = particle_system.angle_over_lifetime.values;
	r32* keys = particle_system.angle_over_lifetime.keys;

	r32_4x start_life = particle_system.particles.start_life[index];

	r32_4x result = r32_4x(values[value_count - 1]);

	i32 found_keys[4] = {};
	i32 found_next_keys[4] = {};

	for (i32 j = 0; j < count; j++)
	{
        i32 i = active_indices[j];
		for (i32 key = 0; key < value_count; key++)
		{
			r64 current_key = keys[key] * start_life.e[i];
			r64 next_key = keys[key + 1] * start_life.e[i];

			if (current_key <= time_spent.e[i] && time_spent.e[i] < next_key)
			{
				found_keys[i] = key;
				found_next_keys[i] = key + 1;
				break;
			}
			else if (key + 1 == value_count - 1 && time_spent.e[i] >= next_key)
			{
				found_keys[i] = key + 1;
				found_next_keys[i] = key + 1;
				break;
			}
		}
	}

	r32_4x current_keys = r32_4x(keys[found_keys[0]], keys[found_keys[1]], keys[found_keys[2]], keys[found_keys[3]]) * start_life;
	r32_4x next_keys = r32_4x(keys[found_next_keys[0]], keys[found_next_keys[1]], keys[found_next_keys[2]], keys[found_next_keys[3]]) * start_life;

	r32_4x start_angle = r32_4x(values[found_keys[0]], values[found_keys[1]], values[found_keys[2]], values[found_keys[3]]);
	r32_4x end_angle = r32_4x(values[found_next_keys[0]], values[found_next_keys[1]], values[found_next_keys[2]], values[found_next_keys[3]]);

	r32_4x t = get_t(time_spent, current_keys, next_keys);

	result = math::lerp(start_angle, t, end_angle);

	return result;
}

static r32_4x get_angle_by_time(ParticleSystemInfo& particle_system, i32 index, r32_4x time_spent)
{
    i32 value_count = particle_system.angle_over_lifetime.value_count;
    r32* values = particle_system.angle_over_lifetime.values;
    r32* keys = particle_system.angle_over_lifetime.keys;

    r32_4x start_life = particle_system.particles.start_life[index];
    r32_4x result = r32_4x(values[value_count - 1]);

    
}

static void update_particles(ParticleWorkData &work_data)
{
    assert(work_data.renderer);
    assert(work_data.info);
    ParticleSystemInfo &info = *work_data.info;

	i32 speed_value_count = info.speed_over_lifetime.value_count;
	i32 color_value_count = info.color_over_lifetime.value_count;
	i32 size_value_count = info.size_over_lifetime.value_count;
	i32 angle_value_count = info.angle_over_lifetime.value_count;
    
    for(i32 alive_index = 0; alive_index < work_data.emitted_this_frame; alive_index++)
    {
        i32 main_index = work_data.emitted_buffer[alive_index];

        r32_4x start_life = info.particles.start_life[main_index];
        r32_4x life = info.particles.life[main_index];

        r32_4x start_mask = equal_mask_epsilon(start_life, life);

        info.particles.life[main_index] -= (r32)work_data.delta_time;

        i32 active_particle_count = 0;
        i32 active_particles[4] = {-1, -1, -1, -1};
        
        for(i32 i = 0; i < 4; i++)
        {
            if(info.particles.life[main_index].e[i] >= 0.0)
            {
                active_particles[active_particle_count++] = i;
            }
        }

        r32_4x alive_mask = ge_mask(info.particles.life[main_index], 0.0f);

        if(any_nz(alive_mask))
        {
            work_data.next_frame_buffer[work_data.next_frame_count++] = main_index;

            __m128i alive = _mm_castps_si128(alive_mask.p);

            __m128i i1 = _mm_shuffle_epi32(alive, 0);
            __m128i i2 = _mm_shuffle_epi32(alive, 1);
            __m128i i3 = _mm_shuffle_epi32(alive, 2);
            __m128i i4 = _mm_shuffle_epi32(alive, 3);

            __m128i count = _mm_set1_epi32(0);
            count = _mm_sub_epi32(count, i1);
            count = _mm_sub_epi32(count, i2);
            count = _mm_sub_epi32(count, i3);
            count = _mm_sub_epi32(count, i4);

            assert(active_particle_count == _mm_cvtsi128_si32(count));

            active_particle_count = _mm_cvtsi128_si32(count);
        }
        else
        {
            work_data.dead_particle_indices[work_data.dead_particle_count++] = main_index;
            assert(work_data.dead_particle_count <= info.max_particles / 4);
            continue;
        }

        life = info.particles.life[main_index];
        r32_4x time_spent = info.particles.start_life[main_index] - life;

        info.particles.direction[main_index] += Vec3_4x(math::Vec3(0.0f, - info.attributes.gravity * (r32)work_data.delta_time, 0.0f));

        Rgba_4x color(0.0f);
        Vec2_4x size(0.0f);
        r32_4x angle(0.0f);

        if(size_value_count > 0)
        {
            size = get_size_by_time(info, main_index, time_spent, &active_particles[0], active_particle_count);
        }
        else
        {
            size = info.particles.start_size[main_index];
        }

        if(angle_value_count > 0)
        {
            angle = get_angle_by_time(info, main_index, time_spent, &active_particles[0], active_particle_count);
        }
        else
        {
            angle = info.particles.start_angle[main_index];
        }

        if(color_value_count > 0)
        {
            color = get_color_by_time(info, main_index, time_spent, &active_particles[0], active_particle_count);
        }
        else
        {
            color = info.attributes.start_color;
        }

        if(speed_value_count > 0)
        {
            r32_4x speed = get_speed_by_time(info, main_index, time_spent, &active_particles[0], active_particle_count);
            info.particles.position[main_index] += info.particles.direction[main_index] * speed * (r32)work_data.delta_time;
        }
        else
        {
            info.particles.position[main_index] += info.particles.direction[main_index] * info.particles.start_speed[main_index] * (r32)work_data.delta_time;
        }

        Vec3_4x final_pos(0.0f);

        if(info.attributes.particle_space == PS_WORLD)
        {
            // @Note: We do both calculations and use a SIMD mask, to not load data out of SIMD registers
            Vec3_4x non_start_final(0.0f);
            Vec3_4x start_final(0.0f);

            Vec3_4x relative_pos = info.particles.relative_position[main_index];

            start_final = info.particles.position[main_index] + info.particles.relative_position[main_index];
            non_start_final = info.particles.position[main_index] + info.transform.position;

            r32_4x res_x = mask_conditional(start_mask, non_start_final.x, start_final.x);
            r32_4x res_y = mask_conditional(start_mask, non_start_final.y, start_final.y);
            r32_4x res_z = mask_conditional(start_mask, non_start_final.z, start_final.z);

            relative_pos.x = mask_conditional(start_mask, r32_4x(info.transform.position.x), relative_pos.x);
            relative_pos.y = mask_conditional(start_mask, r32_4x(info.transform.position.y), relative_pos.y);
            relative_pos.z = mask_conditional(start_mask, r32_4x(info.transform.position.z), relative_pos.z);

            info.particles.relative_position[main_index] = relative_pos;

            final_pos.x = res_x;
            final_pos.y = res_y;
            final_pos.z = res_z;
            
        }
        else
        {
            final_pos = info.particles.position[main_index] + info.transform.position;
            info.particles.relative_position[main_index] = Vec3_4x(info.transform.position);
        }

        float p[4][4];
        float s[4][4];
        float c[4][4];
        float a[4][4];

        r32_4x_to_float4(angle, a[0], a[1], a[2], a[3]);
        vec2_4x_to_float4(size, s[0], s[1], s[2], s[3]);
        vec3_4x_to_float4(final_pos, p[0], p[1], p[2], p[3]);
        vec4_4x_to_float4(color, c[0], c[1], c[2], c[3]);

        for(i32 j = 0; j < active_particle_count; j++)
        {
            i32 i = active_particles[j];

            work_data.offset_buffer[work_data.particle_count].x = p[i][0];
            work_data.offset_buffer[work_data.particle_count].y = p[i][1];
            work_data.offset_buffer[work_data.particle_count].z = p[i][2];

            work_data.color_buffer[work_data.particle_count].x = c[i][0];
            work_data.color_buffer[work_data.particle_count].y = c[i][1];
            work_data.color_buffer[work_data.particle_count].z = c[i][2];
            work_data.color_buffer[work_data.particle_count].w = c[i][3];

            work_data.size_buffer[work_data.particle_count].x = s[i][0];
            work_data.size_buffer[work_data.particle_count].y = s[i][1];

            work_data.angle_buffer[work_data.particle_count] = a[i][0];

            work_data.particle_count++;
        }
        assert(work_data.particle_count < PARTICLE_DATA_SIZE * 4 + 1);
    }
}

static void update_particle_system_job(WorkQueue *work_queue, void *data_ptr)
{
    UpdateParticleSystemWorkData *work_data = (UpdateParticleSystemWorkData*)data_ptr;
    assert(work_data->info);
    assert(work_data->renderer);
    update_particle_system(*work_data->info, work_data->renderer, work_data->delta_time);
}

static void update_particles_job(WorkQueue *work_queue, void* data_ptr)
{
    assert(work_queue);
    ParticleWorkData *work_data = (ParticleWorkData*)data_ptr;
    assert(work_data);
    assert(work_data->renderer);
    assert(work_data->info);

    // @Incomplete: Update
    update_particles(*work_data);
}

// @Note:(Niels): Update the particles that have been emitted in previous and in the current frame
// Kills particles in case they have life <= 0.0 and adds them to the dead particle buffer
// Dead particles are used when emitting in the emit procedure.
// Any particle that is NOT dead, will be written into the alive buffer of the next frame
// This ensures that we can clear the current frame buffer, and only ever iterate over
// particles that are actually alive.
void update_particles(Renderer *renderer, ParticleSystemInfo &particle_system, r64 delta_time, i32 *emitted_buf, i32 *emitted_this_frame, i32* next_frame_buf, i32 *next_frame_count)
{
    if(particle_system.paused)
    {
        return;
    }

    particle_system.particle_count = 0;

    i32 threads = *emitted_this_frame / PARTICLE_DATA_SIZE;
    i32 count = *emitted_this_frame - threads * PARTICLE_DATA_SIZE;
    if(count > 0)
    {
        threads++;
    }

    assert(threads <= particle_system.thread_info_count);
    assert(threads <= particle_system.work_data_count);
    
    for(i32 t = 0; t < threads; t++)
    {
        particle_system.work_datas[t] = {};
        ParticleWorkData &work_data = particle_system.work_datas[t];
        work_data.info = &particle_system;
        work_data.delta_time = delta_time;

        memcpy(&work_data.emitted_buffer[0], emitted_buf + PARTICLE_DATA_SIZE * t, sizeof(i32) * PARTICLE_DATA_SIZE);

        if(t == threads - 1 && count > 0)
        {
            work_data.emitted_this_frame = count;
        }
        else
        {
            work_data.emitted_this_frame = PARTICLE_DATA_SIZE;
        }

        work_data.renderer = renderer;

        assert(particle_system.work_queue);
        platform.add_entry(particle_system.work_queue, update_particles_job, &work_data);
    }

    assert(particle_system.work_queue);
    if(threads > 0)
    {
        platform.complete_all_work(particle_system.work_queue);
    }

    i32 total_particles = 0;
    
    for(i32 t = 0; t < threads; t++)
    {
        ParticleWorkData &work_data = particle_system.work_datas[t];

        total_particles += work_data.particle_count;
        assert(total_particles <= particle_system.max_particles);
        
        for(i32 i = 0; i < work_data.particle_count; i++)
        {
            rendering::add_instance_buffer_value(particle_system.angle_buffer_handle, work_data.angle_buffer[i], renderer);
            rendering::add_instance_buffer_value(particle_system.offset_buffer_handle, work_data.offset_buffer[i], renderer);
            rendering::add_instance_buffer_value(particle_system.size_buffer_handle, work_data.size_buffer[i], renderer);
            rendering::add_instance_buffer_value(particle_system.color_buffer_handle, work_data.color_buffer[i], renderer);
        }

        memcpy(next_frame_buf + (*next_frame_count), &work_data.next_frame_buffer[0], sizeof(i32) * work_data.next_frame_count);
        if(!next_frame_count)
        {
            assert(false);
        }
        *next_frame_count += work_data.next_frame_count;
        memcpy(particle_system.dead_particles + particle_system.dead_particle_count, &work_data.dead_particle_indices[0], sizeof(i32) * work_data.dead_particle_count);
        particle_system.dead_particle_count += work_data.dead_particle_count;
        particle_system.particle_count += work_data.particle_count;
    }

    
    
	*emitted_this_frame = 0;
}

// @Incomplete: Right now we always emit particles for a unique index.
//              Ideally if emitted_count < 4, we want to put the index back into dead particles
//              and then when we emit next time, we look at the index and fill it up.
//              Really, we could just look at the last index used, and see if that index is filled up.
void emit_particle(Renderer *renderer, ParticleSystemInfo &particle_system, i32* alive_buf, i32* count, RandomSeries& entropy, i32 emitted_count)
{
	i32 original_index = find_unused_particle(particle_system);
	assert(original_index != -1);

	math::Vec2 tex_size(1);

	if (particle_system.attributes.texture_handle.handle != 0)
	{
		math::Vec2i i_size = rendering::get_texture_size(particle_system.attributes.texture_handle, renderer);
		r32 max = MAX((r32)i_size.x, (r32)i_size.y);
		tex_size = math::Vec2((r32)i_size.x / max, (r32)i_size.y / max);
	}

	// @Note(Niels): Init particle to the init values saved in the particle system
	if (particle_system.attributes.start_life_time_type == StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
	{
		r32_4x v = random_between_4x(entropy, particle_system.attributes.life.random_between_two_constants.l0, particle_system.attributes.life.random_between_two_constants.l1);
		particle_system.particles.life[original_index] = v;
		particle_system.particles.start_life[original_index] = v;
	}
	else
	{
		particle_system.particles.life[original_index] = particle_system.attributes.life.constant.life_time;
		particle_system.particles.start_life[original_index] = particle_system.attributes.life.constant.life_time;
	}

	if (particle_system.attributes.start_speed_type == StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
	{
		r32_4x v = random_between_4x(entropy, particle_system.attributes.speed.random_between_two_constants.s0, particle_system.attributes.speed.random_between_two_constants.s1);
		particle_system.particles.start_speed[original_index] = v;
	}
	else
	{
		particle_system.particles.start_speed[original_index] = particle_system.attributes.speed.constant.start_speed;
	}

	if (particle_system.attributes.start_size_type == StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
	{
		r32_4x v = random_between_4x(entropy, particle_system.attributes.size.random_between_two_constants.s0, particle_system.attributes.size.random_between_two_constants.s1);
		Vec2_4x size_val = Vec2_4x(v, v) * tex_size;
		particle_system.particles.start_size[original_index] = size_val;
	}
	else
	{
		particle_system.particles.start_size[original_index] = Vec2_4x(particle_system.attributes.size.constant.start_size) * tex_size;
	}

	if (particle_system.attributes.start_angle_type == StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
	{
		r32_4x v = random_between_4x(entropy, particle_system.attributes.angle.random_between_two_constants.a0, particle_system.attributes.angle.random_between_two_constants.a1);
		particle_system.particles.start_angle[original_index] = v;
	}
	else
	{
		particle_system.particles.start_angle[original_index] = particle_system.attributes.angle.constant.start_angle;
	}

	particle_system.particles.relative_position[original_index] = Vec3_4x(0.0f);

    ParticleSpawnInfo spawn_info = {};
    
    switch(particle_system.attributes.emission_module.emitter_func_type)
    {
    case EmissionFuncType::DIRECTION:
    {
        spawn_info = emit_dir(entropy, particle_system.attributes.emission_module.min
                              , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::RANDOM_DIRECTION:
    {
        spawn_info = emit_random_dir(entropy, particle_system.attributes.emission_module.min
                                     , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::SQUARE_2D:
    {
        spawn_info = emit_from_2D_square(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::SQUARE_2D_RANDOM:
    {
        spawn_info = emit_from_2D_square_random(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::SQUARE:
    {
        spawn_info = emit_from_square(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::SQUARE_RANDOM:
    {
        spawn_info = emit_from_square_random(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::DISC:
    {
        spawn_info = emit_from_disc(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::DISC_RANDOM:
    {
        spawn_info = emit_from_disc_random(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::CIRCLE:
    {
        spawn_info = emit_from_circle(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    case EmissionFuncType::CIRCLE_RANDOM:
    {
        spawn_info = emit_from_circle_random(entropy, particle_system.attributes.emission_module.min
                                         , particle_system.attributes.emission_module.max);
    }
    break;
    }
    
	/// @Note(Niels): Generate emission info based on the emitter function

	particle_system.particles.position[original_index] = spawn_info.position;
	Vec3_4x new_direction = spawn_info.direction;

	// @Note(Niels): Now compute the direction based on the direction given in the attributes and the randomly geneerated one
	particle_system.particles.direction[original_index] = math::normalize(particle_system.attributes.direction
		+ new_direction);
	// @Note:(Niels): The current buffer gets the particle being emitted
	// It is passed in, so we only need to check once per frame when the system is updated
	alive_buf[(*count)++] = original_index;
}    

// @Note(Niels): The way we update and choose particles is based on the link below
// https://turanszkij.wordpress.com/2017/11/07/gpu-based-particle-simulation/
void update_particle_system(ParticleSystemInfo& particle_system, Renderer *renderer, r64 delta_time)
{
	if (particle_system.attributes.prewarm && !particle_system.prewarmed)
	{
		particle_system.prewarmed = true;
		update_particle_system(particle_system, renderer, particle_system.attributes.duration);
	}

	// @Note(Niels): We have one buffer that takes the particles from the previous frame
	// plus the particles that are emitted in the current frame.
	// The first time the buffer is empty and only contains emitted particles
	// For any k > 1 where k is the iteration, the buffer works as explained.
	// The second buffer then gets the particles that are still alive this frame
	// and this is also the buffer that contains the particles that are drawn in
	// a frame.
	i32* emitted_alive_buf = nullptr;
	i32* emitted_alive_count = nullptr;
	i32* write_buf = nullptr;
	i32* write_buf_count = nullptr;
	if (particle_system.alive0_active)
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

	if (particle_system.emitting && !particle_system.paused)
	{
		// @Note(Niels): Needed for over-time values

		i32 new_particles = 0;

		// @Note(Niels): Figure out the burst amount if there is any
		// i32 burst_particles = 0;

		// @Note(Niels): Burst code for burst emission.
        // @Incomplete
		// auto value_count = particle_system.attributes.emission_module.burst_over_lifetime.value_count;
		// if (value_count > 0)
		// {
		// 	auto burst_index = particle_system.attributes.emission_module.burst_over_lifetime.current_index;

		// 	if (burst_index < value_count)
		// 	{
		// 		auto &current_burst = particle_system.attributes.emission_module.burst_over_lifetime.values[burst_index];

		// 		auto target_time = current_burst.repeat_interval;

		// 		particle_system.current_emission_time += delta_time;

		// 		if (particle_system.current_emission_time >= target_time)
		// 		{
		// 			burst_particles = current_burst.count;

		// 			if (current_burst.cycle_count > 0 && particle_system.attributes.emission_module.burst_over_lifetime.current_index >= current_burst.cycle_count)
		// 			{
		// 				particle_system.attributes.emission_module.burst_over_lifetime.current_index++;
		// 			}

		// 			particle_system.current_emission_time = 0.0;
		// 		}
		// 	}
		// }

		particle_system.time_spent += delta_time;

		// @Note(Niels): Start figuring out how many particles we need to emit this frame
		r32 per_frame = (r32)((r64)particle_system.attributes.particles_per_second * delta_time);
		particle_system.particles_cumulative += per_frame;

		if (particle_system.particles_cumulative >= 1.0f)
		{
			new_particles = math::floor(particle_system.particles_cumulative);
			particle_system.particles_cumulative = particle_system.particles_cumulative - new_particles;
		}

		// // @Incomplete: Burst is not completely implemented
		// i32 simd_burst_particles = math::multiple_of_number(burst_particles, 4);

		// @Note(Niels): Check if the new amount is below the max and below the amount of dead particles.
		new_particles = MIN(particle_system.max_particles, MIN(new_particles, particle_system.dead_particle_count * 4));

		// @Note(Niels): Emit the particles into the current alive buffer
		// The first time around this buffer is empty, but on any subsequent step
		// there should already be particles from the previous frame, which were
		// written into the then next buffer (which is now the alive buffer)

		if (!particle_system.attributes.looping)
			particle_system.total_emitted += new_particles;

		// @Note: A non-looping particle system stops once its duration has run out
		if (particle_system.time_spent >= particle_system.attributes.duration)
		{
			particle_system.time_spent = 0.0;
			if (!particle_system.attributes.looping)
			{
				particle_system.emitting = false;
			}
		}

		while (new_particles > 0)
		{
			new_particles -= 4;
			if (new_particles < 0)
			{
				emit_particle(renderer, particle_system, emitted_alive_buf, emitted_alive_count, *renderer->particles.entropy, 4 + new_particles);
			}
			else
			{
				emit_particle(renderer, particle_system, emitted_alive_buf, emitted_alive_count, *renderer->particles.entropy, 4);
			}
		}

		// // @Note(Niels): Same goes for burst		
		// for (i32 i = 0; i < simd_burst_particles / 4; i++)
		// {
		// 	emit_particle(renderer, particle_system, emitted_alive_buf, emitted_alive_count, *renderer->particles.entropy, 4);
		// }
	}

    if(!particle_system.paused)
    {
        // @Note:(Niels): We now update the particles in the emitted alive buf (which may contain particles from previous frames that are still alive), while passing in the next buffer,
        // which is now our "write" buffer.

        update_particles(renderer, particle_system, delta_time, emitted_alive_buf, emitted_alive_count, write_buf, write_buf_count);

        // @Note(Niels): if all particles are dead and the system is one-shot we should stop the particle_system
        if (!particle_system.attributes.looping && particle_system.total_emitted > 0 && particle_system.alive0_particle_count == 0 && particle_system.alive1_particle_count == 0)
        {
            particle_system.simulating = false;
        }
    }
}

void update_particle_systems(Renderer *renderer, r64 delta_time)
{
	for (i32 particle_system_index = 0; particle_system_index < renderer->particles.particle_system_count; particle_system_index++)
	{
		ParticleSystemInfo &particle_system = renderer->particles.particle_systems[particle_system_index];
        if(particle_system.simulating)
        {
            particle_system.update_work_data = {};
            UpdateParticleSystemWorkData &work_data = particle_system.update_work_data;
            work_data.info = &particle_system;
            work_data.renderer = renderer;
            work_data.delta_time = delta_time;
            platform.add_entry(renderer->particles.system_work_queue, update_particle_system_job, &work_data);
        }

        math::Rgba* values = particle_system.color_over_lifetime.values;
            
        bezier::CubicBezier curve;
        curve.p0 = math::Vec2(0, 0);
        curve.p1 = math::Vec2(0, values[0].r);
        curve.p2 = math::Vec2(1, values[1].r);
        curve.p3 = math::Vec2(1, values[1].r);
        bezier::draw_bezier_curve(curve, 1.0f, math::Vec2(100.0f), 50.0f);

        rendering::CreateUICommandInfo quad_command = rendering::create_ui_command_info();
        quad_command.transform.position = math::Vec2(100.0f, 100.0f);
        quad_command.transform.scale = math::Vec2(50.0f, curve.p2.y * 50.0f);
        quad_command.z_layer = 5;
        quad_command.scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO;
        quad_command.anchor_flag = rendering::UIAlignment::BOTTOM | rendering::UIAlignment::LEFT;
        quad_command.clip = false;
        quad_command.color = COLOR_WHITE;
        rendering::set_uniform_value(renderer, renderer->render.materials[renderer->render.ui.gradient_material.handle], "c1", values[0].rgb);
        rendering::set_uniform_value(renderer, renderer->render.materials[renderer->render.ui.gradient_material.handle], "c2", values[1].rgb);
        quad_command.custom_material = renderer->render.ui.gradient_material;
        rendering::push_ui_quad(renderer, quad_command);
	}

    if(renderer->particles.particle_system_count > 0)
    {
        platform.complete_all_work(renderer->particles.system_work_queue);
    }
}
// @End


