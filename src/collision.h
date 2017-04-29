static void CheckCollision(box_collider* Collider1, box_collider* Collider2)
{
    if (Collider1->X < Collider2->X + Collider2->Width &&
        Collider1->X + Collider1->Width > Collider2->X &&
        Collider1->Y < Collider2->Y + Collider2->Height &&
        Collider1->Height + Collider1->Y > Collider2->Y) 
    {
        // collision detected!
    }
    
    // filling in the values =>
}