#ifndef RANDOM_H
#define RANDOM_H

struct RandomSeries
{
    r32_4x state;
};

inline RandomSeries random_seed(u32 e0 = 78953890,
                                u32 e1 = 235498,
                                u32 e2 = 893456,
                                u32 e3 = 93453080)
{
    RandomSeries series;
    
    series.state = r32_4x(e0, e1, e2, e3);
    
    return(series);
}

inline r32_4x random_next_u32_4x(RandomSeries& series)
{
    r32_4x result = series.state;
    result ^= shift_left_simd(result, 13);
    result ^= shift_right_simd(result, 17);
    result ^= shift_left_simd(result, 5);
    series.state = result;
    
    return(result);
}

inline u32 random_next_u32(RandomSeries& series)
{
    u32 result = random_next_u32_4x(series).u[0];
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

inline r32_4x random_unilateral_4x(RandomSeries& series)
{
    r32 divisor = 1.0f / (r32)u32max;
    r32_4x result = u32_to_r32(random_next_u32_4x(series)) * r32_4x(divisor);
    
    return(result);
}


inline r32_4x random_bilateral_4x(RandomSeries& series)
{
    r32_4x result = r32_4x(2.0f) * random_unilateral_4x(series) - 1.0f;
    
    return(result);
}

inline r32 random_bilateral(RandomSeries& series)
{
    r32 result = 2.0f * random_unilateral(series) - 1.0f;
    
    return(result);
}


inline r32_4x random_between_4x(RandomSeries& series, r32 min, r32 max)
{
    r32_4x result = min + math::lerp(r32_4x(min), random_unilateral_4x(series), r32_4x(max));
    return result;
}

inline i32_4x random_between_4x(RandomSeries& series, i32 min, i32 max)
{
    r32_4x s = random_next_u32_4x(series);
    i32 result_1 = min + i32(s.u[0] % ((max + 1) - min));
    i32 result_2 = min + i32(s.u[1] % ((max + 1) - min));
    i32 result_3 = min + i32(s.u[2] % ((max + 1) - min));
    i32 result_4 = min + i32(s.u[3] % ((max + 1) - min));
    i32_4x result = i32_4x(result_1, result_2, result_3, result_4);
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

inline Vec2_4x random_from_disc(RandomSeries& series, r32 _radius)
{
    Vec2_4x result;
    
    r32_4x angle = random_between_4x(series, 0.0f, 360.0f) * r32_4x(DEGREE_IN_RADIANS);
    r32_4x radius = random_between_4x(series, 0.0f, _radius);
    result.x = (radius * r32_4x(cos(angle.e[0]),cos(angle.e[1]), cos(angle.e[2]), cos(angle.e[3])));
    result.y = (radius * r32_4x(sin(angle.e[0]),sin(angle.e[1]), sin(angle.e[2]), sin(angle.e[3])));
    
    return result;
}

inline r32_4x random_unilateral_4ximd(RandomSeries& series)
{
    r32_4x divisor = r32_4x(1.0f / (r32)i32max);
    // @Note(Niels): Ok to cast to u32 since i32max is always u32max / 2 and never negative (no underflow)
    r32_4x result = divisor * (u32_to_r32(random_next_u32_4x(series) & r32_4x((u32)i32max)));
    return result;
}

inline Vec3_4x random_direction(RandomSeries& series)
{
    Vec3_4x result(0.0f);
    
    result.x = random_between_4x(series, 0.0f, 1.0f);
    result.y = random_between_4x(series, 0.0f, 1.0f);
    result.z = random_between_4x(series, 0.0f, 1.0f);
    
    return math::normalize(result);
}

inline Vec3_4x random_rect(RandomSeries& series, r32 min, r32 max)
{
    Vec3_4x result(0.0f);
    
    result.x = random_between_4x(series, min, max);
    result.y = _mm_set1_ps(0.0f);
    
    result.z = random_between_4x(series, min, max);
    
    return result;
}

inline Vec3_4x random_disc(RandomSeries& series, r32 radius)
{
    Vec3_4x result(0.0f);
    
    Vec2_4x vec = random_from_disc(series, radius);
    
    result.x = vec.x;
    result.y = _mm_set1_ps(0.0f);
    
    result.z = vec.y;
    
    return result;
}


#endif