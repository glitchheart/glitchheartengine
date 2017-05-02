//TODO(daniel) doesn't take rotations into account. need SAT for this
static bool32 CheckCollision(entity* Entity1, entity* Entity2)
{
    collision_rect Collider1 = Entity1->CollisionRect;
    collision_rect Collider2 = Entity2->CollisionRect;
    if (Collider1.X + Entity1->Position.x < Collider2.X + Entity2->Position.x + Collider2.Width &&
        Collider1.X + Entity1->Position.x + Collider1.Width > Collider2.X + Entity2->Position.x &&
        Collider1.Y + Entity1->Position.y < Collider2.Y + Entity2->Position.y + Collider2.Height &&
        Collider1.Height + Collider1.Y + Entity1->Position.y > Collider2.Y + Entity2->Position.y) 
    {
        auto ColliderDir = glm::vec2((Entity1->Position.x + Collider1.X + Collider1.Width) - (Entity2->Position.x + Collider2.X + Collider2.Width),
                                     (Entity1->Position.y + Collider1.Y + Collider1.Height) - (Entity2->Position.y + Collider2.Y + Collider2.Height));
        
        if(ColliderDir.x > 0)
        {
            Entity1->CollisionRect.Left = true;
            Entity2->CollisionRect.Right = true;
        } 
        else
        {
            Entity1->CollisionRect.Right = true;
            Entity2->CollisionRect.Left = true;
        }
        
        if(ColliderDir.y > 0)
        {
            Entity1->CollisionRect.Top = true;
            Entity2->CollisionRect.Bottom = true;
        }
        else
        {
            Entity1->CollisionRect.Bottom = true;
            Entity2->CollisionRect.Top = true;
        }
        
        return 1;
    }
    return 0;
}

static void AABBMin(collision_AABB* Coll)
{
    Coll->Min = glm::vec2(Coll->Center.x - Coll->Extents.x, Coll->Center.y - Coll->Extents.y);
}

static void AABBMax(collision_AABB* Coll)
{
    Coll->Max = glm::vec2(Coll->Center.x + Coll->Extents.x, Coll->Center.y + Coll->Extents.y);
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
    glm::vec2 TopLeft = glm::vec2(Coll->Min.x - Other->Max.x,Coll->Min.y - Other->Min.y);
    glm::vec2 FullSize = glm::vec2(Coll->Size.x + Other->Size.x, Coll->
                                   Size.y + Other->Size.y);
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
    real32 MinDist = glm::abs(Point.x - Coll->Min.x);
    *Out = glm::vec2(Coll->Min.x, Point.y);
    if(glm::abs(Coll->Max.x - Point.x) < MinDist)
    {
        MinDist = glm::abs(Coll->Max.x - Point.x);
        *Out= glm::vec2(Coll->Max.x, Point.y);
    }
    if(glm::abs(Coll->Max.y - Point.y) < MinDist)
    {
        MinDist = glm::abs(Coll->Max.x - Point.y);
        *Out= glm::vec2(Point.x, Coll->Max.y);
    }
    if(glm::abs(Coll->Min.y - Point.y) < MinDist)
    {
        MinDist = glm::abs(Coll->Min.y - Point.y);
        *Out= glm::vec2(Point.x, Coll->Min.y);
    }
}


