//TODO(daniel) doesn't take rotations into account. need SAT for this
static bool32 CheckCollision(collision_rect* Collider1, collision_rect* Collider2)
{
    if (Collider1->X < Collider2->X + Collider2->Width &&
        Collider1->X + Collider1->Width > Collider2->X &&
        Collider1->Y < Collider2->Y + Collider2->Height &&
        Collider1->Height + Collider1->Y > Collider2->Y) 
    {
        return 1;
    }
    return 0;
}


