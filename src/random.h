#ifndef RANDOM_H
#define RANDOM_H

struct RandomSeries
{
    r32_4x state;
};

inline void random_seed(RandomSeries& series, u32 e0 = 78953890,
                                u32 e1 = 235498,
                                u32 e2 = 893456,
                                u32 e3 = 93453080)
{
    series.state = r32_4x(e0, e1, e2, e3);
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

inline r32 random_bilateral(RandomSeries& series)
{
    r32 result = 2.0f * random_unilateral(series) - 1.0f;
    
    return(result);
}

inline r32 random_between(RandomSeries& series, r32 min, r32 max)
{
    r32 result = math::lerp(min, random_unilateral(series), max);
    return result;
}

inline i32 random_between(RandomSeries& series, i32 min, i32 max)
{
    i32 result = min + (i32)(random_next_u32(series) % ((max + 1) - min));
    return result;
}

inline r32_4x random_unilateral_4x(RandomSeries& series)
{
    r32_4x divisor = r32_4x(1.0f / (r32)i32max);
    r32_4x result = divisor * (u32_to_r32(random_next_u32_4x(series) & r32_4x((u32)i32max)));
    return(result);
}

inline r32_4x random_bilateral_4x(RandomSeries& series)
{
    r32_4x result = r32_4x(2.0f) * random_unilateral_4x(series) - 1.0f;
    
    return(result);
}

inline r32_4x random_between_4x(RandomSeries& series, r32 min, r32 max)
{
    r32_4x r = r32_4x(random_unilateral_4x(series));
    r32_4x result = math::lerp(r32_4x(min), r, r32_4x(max));
    
    return(result);
}

inline r64_4x random_between_4x(RandomSeries& series, r64 min, r64 max)
{
    r64_4x r = r64_4x(random_unilateral_4x(series));
    r64_4x result = math::lerp(r64_4x(min), r, r64_4x(max));
    
    return(result);
}

/* // @Note:(Niels): No simd remainder/mod function :( */
/* inline i32_4x random_between_4x(RandomSeries& series, i32 min, i32 max) */
/* { */
/*     r32_4x s = random_next_u32_4x(series); */
/*     i32 result_1 = min + i32(s.u[0] % ((max + 1) - min)); */
/*     i32 result_2 = min + i32(s.u[1] % ((max + 1) - min)); */
/*     i32 result_3 = min + i32(s.u[2] % ((max + 1) - min)); */
/*     i32 result_4 = min + i32(s.u[3] % ((max + 1) - min)); */
/*     i32_4x result = i32_4x(result_1, result_2, result_3, result_4); */
/*     return(result); */
/* } */

inline Vec2_4x random_from_disc(RandomSeries& series, r32 _radius)
{
    Vec2_4x result;
    
    r32_4x angle = random_between_4x(series, 0.0f, 360.0f) * r32_4x(DEGREE_IN_RADIANS);
    r32_4x radius = random_between_4x(series, 0.0f, _radius);
    result.x = (radius * r32_4x((r32)cos(angle.e[0]),(r32)cos(angle.e[1]), (r32)cos(angle.e[2]), (r32)cos(angle.e[3])));
    result.y = (radius * r32_4x((r32)sin(angle.e[0]),(r32)sin(angle.e[1]), (r32)sin(angle.e[2]), (r32)sin(angle.e[3])));
    
    return result;
}

inline Vec2_4x random_from_circle(RandomSeries& series, r32 _radius)
{
    Vec2_4x result;
    
    r32_4x angle = random_between_4x(series, 0.0f, 360.0f) * r32_4x(DEGREE_IN_RADIANS);
    r32_4x radius = random_between_4x(series, _radius, _radius);
    result.x = (radius * r32_4x((r32)cos(angle.e[0]), (r32)cos(angle.e[1]), (r32)cos(angle.e[2]), (r32)cos(angle.e[3])));
    result.y = (radius * r32_4x((r32)sin(angle.e[0]), (r32)sin(angle.e[1]), (r32)sin(angle.e[2]), (r32)sin(angle.e[3])));
    
    return result;
}

inline Vec3_4x random_direction_4x(RandomSeries& series)
{
    Vec3_4x result(0.0f);
    
    result.x = random_between_4x(series, -1.0f, 1.0f);
    result.y = random_between_4x(series, -1.0f, 1.0f);
    result.z = random_between_4x(series, -1.0f, 1.0f);
    
    return math::normalize(result);
}

inline Vec3_4x random_rect_4x(RandomSeries& series, r32 _min, r32 _max)
{
    Vec3_4x result(0.0f);
    
    r32 min = _min - (_min + _max) / 2.0f;
    r32 max = _max - (_min + _max) / 2.0f;

    result.x = random_between_4x(series, min, max);
    result.y = _mm_set1_ps(0.0f);
    
    result.z = random_between_4x(series, min, max);
    
    return result;
}

inline math::Vec3 get_point_on_square(r32 r, r32 a, r32 b, r32 x0, r32 x1, r32 y0, r32 y1)
{
    math::Vec3 result(0.0f);
    
    if(r < a)
    {
        result.x = x0;
        result.z = y0 + a - r;
    }
    else if(r < a + b)
    {
        result.x = x0 + r - a;
        result.z = y0 + a;
    }
    else if(r < (a + b) + a)
    {
        result.x = x0 + b;
        result.z = y0 + r - (a + b);
    }
    else
    {
        result.x = x0 + r - (a + b + a);
        result.z = y0;
    }
    
    return result;
}

inline Vec3_4x random_outer_rect_4x(RandomSeries& series, r32 _x0, r32 _x1, r32 _y0, r32 _y1)
{
    Vec3_4x result(0.0f);
    
    
    r32 x0 = _x0 - ((_x1 - _x0) / 2.0f);
    r32 x1 = _x1 - ((_x1 - _x0) / 2.0f);
    r32 y0 = _y0 - ((_y1 - _y0) / 2.0f);
    r32 y1 = _y1 - ((_y1 - _y0) / 2.0f);
    
    r32 a = x1 - x0;
    r32 b = y1 - y0;
    
    // @Note:(Niels): Assumes x1 > x0 and y1 > y0
    r32 line_length = a * 2.0f + b * 2.0f;
    
    r32_4x r = random_between_4x(series, 0.0f, line_length);
    
    result = Vec3_4x(get_point_on_square(r.e[0], a, b, x0, x1, y0, y1),
                     get_point_on_square(r.e[1], a, b, x0, x1, y0, y1),
                     get_point_on_square(r.e[2], a, b, x0, x1, y0, y1),
                     get_point_on_square(r.e[3], a, b, x0, x1, y0, y1));
    
    return result;
}

inline Vec3_4x random_disc_4x(RandomSeries& series, r32 radius)
{
    Vec3_4x result(0.0f);
    
    Vec2_4x vec = random_from_disc(series, radius);
    
    result.x = vec.x;
    result.y = _mm_set1_ps(0.0f);
    
    result.z = vec.y;
    
    return result;
}

inline Vec3_4x random_circle_4x(RandomSeries& series, r32 radius)
{
    Vec3_4x result(0.0f);
    
    Vec2_4x vec = random_from_circle(series, radius);
    
    result.x = vec.x;
    result.y = _mm_set1_ps(0.0f);
    
    result.z = vec.y;
    
    return result;
}


#endif
