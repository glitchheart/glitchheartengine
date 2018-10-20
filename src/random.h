#ifndef RANDOM_H
#define RANDOM_H

struct RandomSeries
{
    S_r32 state;
};

inline RandomSeries random_seed(u32 e0 = 78953890,
                                u32 e1 = 235498,
                                u32 e2 = 893456,
                                u32 e3 = 93453080)
{
    RandomSeries series;
    
    series.state = S_r32(e0, e1, e2, e3);
    
    return(series);
}

inline S_r32 random_next_u32_s(RandomSeries& series)
{
    S_r32 result = series.state;
    result ^= shift_left_simd(result, 13);
    result ^= shift_right_simd(result, 17);
    result ^= shift_left_simd(result, 5);
    series.state = result;
    
    return(result);
}

inline u32 random_next_u32(RandomSeries& series)
{
    u32 result = random_next_u32_s(series).u[0];
    return(result);
}

inline u32 random_choice(RandomSeries& series, u32 choice_count)
{
    u32 result = (random_next_u32(series) % choice_count);
    return(result);
}

inline r32 random_unilateral(RandomSeries& series)
{
    r32 divisor = 1.0f / (r32)u32max;
    r32 result = divisor * (r32)random_next_u32(series);
    
    return(result);
}

inline S_r32 random_unilateral_s(RandomSeries& series)
{
    r32 divisor = 1.0f / (r32)u32max;
    S_r32 result = u32_to_r32(random_next_u32_s(series)) * S_r32(divisor);
    
    return(result);
}


inline S_r32 random_bilateral_s(RandomSeries& series)
{
    S_r32 result = S_r32(2.0f) * random_unilateral_s(series) - 1.0f;
    
    return(result);
}

inline r32 random_bilateral(RandomSeries& series)
{
    r32 result = 2.0f * random_unilateral(series) - 1.0f;
    
    return(result);
}


inline S_r32 random_between_s(RandomSeries& series, r32 min, r32 max)
{
    S_r32 result = min + math::lerp(S_r32(min), random_unilateral_s(series), S_r32(max));
    return result;
}

inline S_i32 random_between_s(RandomSeries& series, i32 min, i32 max)
{
    S_r32 s = random_next_u32_s(series);
    i32 result_1 = min + i32(s.u[0] % ((max + 1) - min));
    i32 result_2 = min + i32(s.u[1] % ((max + 1) - min));
    i32 result_3 = min + i32(s.u[2] % ((max + 1) - min));
    i32 result_4 = min + i32(s.u[3] % ((max + 1) - min));
    S_i32 result = S_i32(result_1, result_2, result_3, result_4);
    return result;
}

inline r32 random_between(RandomSeries& series, r32 min, r32 max)
{
    r32 result = min + math::lerp(min, random_unilateral(series), max);
    return result;
}

inline i32 random_between(RandomSeries& series, i32 min, i32 max)
{
    i32 result = min + (i32)(random_next_u32(series) % ((max + 1) - min));
    return result;
}

inline S_Vec2 random_from_disc(RandomSeries& series, r32 _radius)
{
    S_Vec2 result;
    
    S_r32 angle = random_between_s(series, 0.0f, 360.0f) * S_r32(DEGREE_IN_RADIANS);
    S_r32 radius = random_between_s(series, 0.0f, _radius);
    result.x = (radius * S_r32(cos(angle.e[0]),cos(angle.e[1]), cos(angle.e[2]), cos(angle.e[3])));
    result.y = (radius * S_r32(sin(angle.e[0]),sin(angle.e[1]), sin(angle.e[2]), sin(angle.e[3])));
    
    return result;
}

inline S_r32 random_unilateral_simd(RandomSeries& series)
{
    S_r32 divisor = S_r32(1.0f / (r32)i32max);
    S_r32 result = divisor * (u32_to_r32(random_next_u32_s(series) & S_r32(i32max)));
    return result;
}

inline S_Vec3 random_direction(RandomSeries& series)
{
    S_Vec3 result(0.0f);
    
    result.x = random_between_s(series, 0.0f, 1.0f);
    result.y = random_between_s(series, 0.0f, 1.0f);
    result.z = random_between_s(series, 0.0f, 1.0f);
    
    return math::normalize(result);
}

inline S_Vec3 random_rect(RandomSeries& series, r32 min, r32 max)
{
    S_Vec3 result(0.0f);
    
    result.x = random_between_s(series, min, max);
    result.y = _mm_set1_ps(0.0f);
    
    result.z = random_between_s(series, min, max);
    
    return result;
}

inline S_Vec3 random_disc(RandomSeries& series, r32 radius)
{
    S_Vec3 result(0.0f);
    
    S_Vec2 vec = random_from_disc(series, radius);
    
    result.x = vec.x;
    result.y = _mm_set1_ps(0.0f);
    
    result.z = vec.y;
    
    return result;
}


#endif