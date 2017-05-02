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


