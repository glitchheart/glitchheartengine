
static void AABBMin(collision_AABB* Coll)
{
    Coll->Min = Coll->Center - Coll->Extents;
}

static void AABBMax(collision_AABB* Coll)
{
    Coll->Max = Coll->Center + Coll->Extents;
}

static void AABBSize(collision_AABB* Coll)
{
    Coll->Size = glm::vec2(Coll->Extents.x * 2, Coll->Extents.y * 2);
}

static void MinkowskiDifference(collision_AABB* Coll, collision_AABB* Other, collision_AABB* Out)
{
    AABBMin(Coll);
    AABBMax(Coll);
    AABBSize(Coll);
    AABBMin(Other);
    AABBMax(Other);
    AABBSize(Other);
    glm::vec2 TopLeft = Coll->Min - Other->Max;
    glm::vec2 FullSize = Coll->Size + Other->Size;
    glm::vec2 Center = glm::vec2(TopLeft.x + (FullSize.x / 2), TopLeft.y + (FullSize.y /2));
    glm::vec2 Extents = glm::vec2(FullSize.x / 2, FullSize.y / 2);
    Out->Center = Center;
    Out->Extents = Extents;
    AABBMin(Out);
    AABBMax(Out);
    AABBSize(Out);
}

static void ClosestPointsOnBoundsToPoint(collision_AABB* Coll, glm::vec2 Point, glm::vec2* Out)
{  
    real32 MinDist = Abs(Point.x - Coll->Min.x);
    
    *Out = glm::vec2(Coll->Min.x, Point.y);
    
    if(Abs(Coll->Max.x - Point.x) < MinDist)
    {
        MinDist = Abs(Coll->Max.x - Point.x);
        *Out = glm::vec2(Coll->Max.x, Point.y);
    }
    if(Abs(Coll->Max.y - Point.y) < MinDist)
    {
        MinDist = Abs(Coll->Max.y - Point.y);
        *Out = glm::vec2(Point.x, Coll->Max.y);
    }
    if(Abs(Coll->Min.y - Point.y) < MinDist)
    {
        MinDist = Abs(Coll->Min.y - Point.y);
        *Out = glm::vec2(Point.x, Coll->Min.y);
    }
}

static real32 GetRayIntersectionFractionOfFirstRay(glm::vec2 OriginA, glm::vec2 EndA,glm::vec2 OriginB, glm::vec2 EndB)
{
    glm::vec2 R = EndA - OriginA;
    glm::vec2 S = EndB - OriginB;
    
    real32 Numerator = glm::dot((OriginB - OriginA),  R);
    real32 Denominator = glm::dot(R , S);
    
    if(Numerator == 0 && Denominator == 0)
    {
        return INFINITY;
    }
    
    if(Denominator == 0)
    {
        return INFINITY;
    }
    
    real32 U = Numerator / Denominator;
    real32 T = ((glm::dot(OriginA / OriginB,S))) / Denominator;
    
    if ((T >= 0) && (T <= 1) && (U >= 0) && (U <= 1))
    {
        return T;
    }
    
    return INFINITY;
}

static real32 GetRayIntersectionFraction(collision_AABB* Coll, glm::vec2 Origin, glm::vec2 Direction)
{
    AABBMin(Coll);
    AABBMax(Coll);
    glm::vec2 End = Origin + Direction;
    
    real32 MinT = GetRayIntersectionFractionOfFirstRay(Origin,End,glm::vec2(Coll->Min.x, Coll->Min.y),
                                                       glm::vec2(Coll->Min.x, Coll->Max.y));
    real32 X;
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, glm::vec2(Coll->Min.x, Coll->Max.y),
                                             glm::vec2(Coll->Max.x, Coll->Max.y));
    if(X < MinT)
    {
        MinT = X;
    }
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, glm::vec2(Coll->Max.x, Coll->Max.y),
                                             glm::vec2(Coll->Max.x, Coll->Min.y));
    if(X < MinT)
    {
        MinT = X;
    }
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, glm::vec2(Coll->Max.x, Coll->Min.y),
                                             glm::vec2(Coll->Min.x, Coll->Min.y));
    if(X < MinT)
    {
        MinT = X;
    }
    return MinT;
}

