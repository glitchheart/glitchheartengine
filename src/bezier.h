#ifndef BEZIER_H
#define BEZIER_H

namespace bezier
{
    struct CubicBezier
    {
        math::Vec2 p0;
        math::Vec2 p1;
        math::Vec2 p2;
        math::Vec2 p3;
    };

    struct QuadraticBezier
    {
        math::Vec2 p0;
        math::Vec2 p1;
        math::Vec2 p2;        
    };
}

#endif
