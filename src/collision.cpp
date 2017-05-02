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

static void InitAABB(collision_AABB* Coll)
{
    Coll->Min = glm::vec2(Coll->Center.x - Coll->Extents.x, Coll->Center.y - Coll->Extents.y);
    Coll->Max = glm::vec2(Coll->Center.x + Coll->Extents.x, Coll->Center.y + Coll->Extents.y);
    Coll->Size = glm::vec2(Coll->Extents.x * 2, Coll->Extents.y * 2);
}

static void MinkowskiDifference(collision_AABB Coll, collision_AABB Other, collision_AABB* Out)
{
    glm::vec2 TopLeft = glm::vec2(Coll.Min.x - Other.Max.x,Coll.Min.y - Other.Min.y);
    glm::vec2 FullSize = glm::vec2(Coll.Size.x + Other.Size.x, Coll.Size.y + Other.Size.y);
    glm::vec2 Center = glm::vec2(TopLeft.x + (FullSize.x / 2), TopLeft.y + (FullSize.y /2));
    glm::vec2 Extents = glm::vec2(FullSize.x / 2, FullSize.y / 2);
    Out->Center = Center;
    Out->Extents = Extents;
}


