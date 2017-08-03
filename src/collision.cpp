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
    Coll->Size = math::v2(Coll->Extents.x * 2, Coll->Extents.y * 2);
}

static void MinkowskiDifference(collision_AABB* Coll, collision_AABB* Other, collision_AABB* Out)
{
    AABBMin(Coll);
    AABBMax(Coll);
    AABBSize(Coll);
    AABBMin(Other);
    AABBMax(Other);
    AABBSize(Other);
    math::v2 TopLeft = Coll->Min - Other->Max;
    math::v2 FullSize = Coll->Size + Other->Size;
    math::v2 Center = math::v2(TopLeft.x + (FullSize.x / 2), TopLeft.y + (FullSize.y /2));
    math::v2 Extents = math::v2(FullSize.x / 2, FullSize.y / 2);
    Out->Center = Center;
    Out->Extents = Extents;
    AABBMin(Out);
    AABBMax(Out);
    AABBSize(Out);
}

static void ClosestPointsOnBoundsToPoint(collision_AABB* Coll, math::v2 Point, math::v2* Out)
{  
    r32 MinDist = Abs(Point.x - Coll->Min.x);
    
    *Out = math::v2(Coll->Min.x, Point.y);
    
    if(Abs(Coll->Max.x - Point.x) < MinDist)
    {
        MinDist = Abs(Coll->Max.x - Point.x);
        *Out = math::v2(Coll->Max.x, Point.y);
    }
    
    if(Abs(Coll->Max.y - Point.y) < MinDist)
    {
        MinDist = Abs(Coll->Max.y - Point.y);
        *Out = math::v2(Point.x, Coll->Max.y);
    }
    if(Abs(Coll->Min.y - Point.y) < MinDist)
    {
        MinDist = Abs(Coll->Min.y - Point.y);
        *Out = math::v2(Point.x, Coll->Min.y);
    }
}

static r32 GetRayIntersectionFractionOfFirstRay(math::v2 OriginA, math::v2 EndA,math::v2 OriginB, math::v2 EndB)
{
    math::v2 R = EndA - OriginA;
    math::v2 S = EndB - OriginB;
    
    r32 Numerator = Dot((OriginB - OriginA),  R);
    r32 Denominator = Dot(R , S);
    
    if(Numerator == 0 && Denominator == 0)
    {
        return INFINITY;
    }
    
    if(Denominator == 0)
    {
        return INFINITY;
    }
    
    r32 U = Numerator / Denominator;
    r32 T = ((Dot(OriginA / OriginB,S))) / Denominator;
    
    if ((T >= 0) && (T <= 1) && (U >= 0) && (U <= 1))
    {
        return T;
    }
    
    return INFINITY;
}

r32 GetRayIntersectionFraction(collision_AABB* Coll, math::v2 Origin, math::v2 Direction)
{
    AABBMin(Coll);
    AABBMax(Coll);
    math::v2 End = Origin + Direction;
    
    r32 MinT = GetRayIntersectionFractionOfFirstRay(Origin,End,math::v2(Coll->Min.x, Coll->Min.y),
                                                    math::v2(Coll->Min.x, Coll->Max.y));
    r32 X;
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, math::v2(Coll->Min.x, Coll->Max.y),
                                             math::v2(Coll->Max.x, Coll->Max.y));
    if(X < MinT)
    {
        MinT = X;
    }
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, math::v2(Coll->Max.x, Coll->Max.y),
                                             math::v2(Coll->Max.x, Coll->Min.y));
    if(X < MinT)
    {
        MinT = X;
    }
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, math::v2(Coll->Max.x, Coll->Min.y),
                                             math::v2(Coll->Min.x, Coll->Min.y));
    if(X < MinT)
    {
        MinT = X;
    }
    return MinT;
}

void CheckWeaponCollision(game_state* GameState, entity_weapon* Entity, collision_info* CollisionInfo)
{
    CollisionInfo->OtherCount = -1;
    
    math::v2 PV;
    CollisionInfo->OtherCount = 0;
    
    for(u32 OtherEntityIndex = 0;
        OtherEntityIndex < GameState->EntityCount;
        OtherEntityIndex++)
    {
        entity* OtherEntity = &GameState->Entities[OtherEntityIndex];
        
        if(!(OtherEntity->Layer & Entity->IgnoreLayers)
           && !(Entity->Layer & OtherEntity->IgnoreLayers)
           && !OtherEntity->IsKinematic && OtherEntity->Active)
        {
            if(OtherEntity->HasHitTrigger)
            {
                collision_AABB MdHit;
                MinkowskiDifference(&OtherEntity->HitTrigger, &Entity->CollisionAABB, &MdHit);
                if(MdHit.Min.x <= 0 &&
                   MdHit.Max.x >= 0 &&
                   MdHit.Min.y <= 0 &&
                   MdHit.Max.y >= 0)
                {
                    CollisionInfo->Other[CollisionInfo->OtherCount++] = OtherEntity;
                    
                    OtherEntity->HitTrigger.IsColliding = true;
                }
                else 
                {
                    OtherEntity->HitTrigger.IsColliding = false;
                }
            }
        }
    }
}

static void CheckCollision(game_state* GameState, entity* Entity, collision_info* CollisionInfo)
{
    CollisionInfo->OtherCount = -1;
    
    if(!Entity->IsKinematic && Entity->Active)
    {
        Entity->CollisionAABB.Center = math::v2(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale + Entity->CollisionAABB.Offset.y);
        
        if(Entity->Type == Entity_Enemy)
        {
            Entity->Enemy.EnemyCollider.Center = math::v2(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->Enemy.EnemyCollider.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale + Entity->Enemy.EnemyCollider.Offset.y);
        }
        
        if(Entity->HasHitTrigger)
        {
            Entity->HitTrigger.Center = math::v2(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->HitTrigger.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale + Entity->HitTrigger.Offset.y);
        }
        
        math::v2 PV;
        CollisionInfo->OtherCount = 0;
        
        for(u32 OtherEntityIndex = 0;
            OtherEntityIndex < GameState->EntityCount;
            OtherEntityIndex++)
        {
            entity* OtherEntity = &GameState->Entities[OtherEntityIndex];
            
            if(!(OtherEntity->Layer & Entity->IgnoreLayers) && !(Entity->Layer & OtherEntity->IgnoreLayers) 
               && OtherEntity->EntityIndex != Entity->EntityIndex 
               && !OtherEntity->IsKinematic && OtherEntity->Active)
            {
                if(OtherEntity->HasHitTrigger)
                {
                    collision_AABB MdHit;
                    MinkowskiDifference(&OtherEntity->HitTrigger, &Entity->CollisionAABB, &MdHit);
                    if(MdHit.Min.x <= 0 &&
                       MdHit.Max.x >= 0 &&
                       MdHit.Min.y <= 0 &&
                       MdHit.Max.y >= 0)
                    {
                        CollisionInfo->Other[CollisionInfo->OtherCount++] = OtherEntity;
                        
                        OtherEntity->HitTrigger.IsColliding = true;
                    }
                    else 
                    {
                        OtherEntity->HitTrigger.IsColliding = false;
                    }
                }
                
                collision_AABB Md;
                //if(Entity->Type == Entity_Enemy && OtherEntity->Type == Entity_Enemy)
                //{
                //MinkowskiDifference(&OtherEntity->Enemy.EnemyCollider, &Entity->Enemy.EnemyCollider, &Md);
                //}
                //else
                MinkowskiDifference(&OtherEntity->CollisionAABB, &Entity->CollisionAABB, &Md);
                
                if(Md.Min.x <= 0 &&
                   Md.Max.x >= 0 &&
                   Md.Min.y <= 0 &&
                   Md.Max.y >= 0)
                {
                    CollisionInfo->Other[CollisionInfo->OtherCount++] = OtherEntity;
                    
                    Entity->IsColliding = true;
                    Entity->CollisionAABB.IsColliding = true;
                    
                    if(!Entity->CollisionAABB.IsTrigger && !OtherEntity->CollisionAABB.IsTrigger)
                    {
                        OtherEntity->IsColliding = true;
                        OtherEntity->CollisionAABB.IsColliding = true;
                        
                        AABBMin(&Md);
                        AABBMax(&Md);
                        AABBSize(&Md);
                        math::v2 PenetrationVector;
                        ClosestPointsOnBoundsToPoint(&Md, math::v2(0,0), &PenetrationVector);
                        
                        if(Abs(PenetrationVector.x) > Abs(PenetrationVector.y))
                        {
                            if(PenetrationVector.x > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                            else if(PenetrationVector.x < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                        }
                        else
                        {
                            if(PenetrationVector.y > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                            else if(PenetrationVector.y < 0) 
                                CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                        }
                        
                        if(PenetrationVector.x != 0)
                        {
                            PV.x = PenetrationVector.x;
                        }
                        
                        if(PenetrationVector.y != 0)
                        {
                            PV.y = PenetrationVector.y;
                        }
                    }
                }
                else 
                {
                    Entity->IsColliding = false;
                    Entity->CollisionAABB.IsColliding = false;
                }
            }
        }
        
        level* Level = &GameState->CurrentLevel;
        
        i32 XPos = (i32)(Entity->Position.x + Entity->Center.x * Entity->Scale);
        i32 YPos = (i32)(Entity->Position.y + Entity->Center.y * Entity->Scale);
        
        //@Improvement Is it necessary to go 2 tiles out?
        i32 MinX = Max(0, XPos - 2);
        i32 MaxX = Max(0, Min((i32)Level->Tilemap.Width, XPos + 2));
        i32 MinY = Max(0, YPos - 2);
        i32 MaxY = Max(0, Min((i32)Level->Tilemap.Height, YPos + 2));
        
        //check tile collision
        for(i32 X = MinX; X < MaxX; X++)
        {
            for(i32 Y = MinY; Y < MaxY; Y++)
            {
                tile_data Tile = Level->Tilemap.Data[1][X][Y];
                
                if(Tile.IsSolid)
                {
                    //@Cleanup we have to move this to a separate function, because it is used in entity collision and tile collision
                    collision_AABB Md;
                    MinkowskiDifference(&Tile.CollisionAABB, &Entity->CollisionAABB, &Md);
                    if(Md.Min.x <= 0 &&
                       Md.Max.x >= 0 &&
                       Md.Min.y <= 0 &&
                       Md.Max.y >= 0)
                    {
                        Entity->IsColliding = true;
                        Entity->CollisionAABB.IsColliding = true;
                        
                        //calculate what side is colliding
                        auto OtherPosition = Tile.CollisionAABB.Center;
                        auto OtherExtents = Tile.CollisionAABB.Extents;
                        auto Position = Entity->CollisionAABB.Center;
                        auto Extents = Entity->CollisionAABB.Extents;
                        
                        AABBMin(&Md);
                        AABBMax(&Md);
                        AABBSize(&Md);
                        math::v2 PenetrationVector;
                        ClosestPointsOnBoundsToPoint(&Md, math::v2(0,0), &PenetrationVector);
                        
                        if(Abs(PenetrationVector.x) > Abs(PenetrationVector.y))
                        {
                            if(PenetrationVector.x > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                            else if(PenetrationVector.x < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                        }
                        else
                        {
                            if(PenetrationVector.y < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                            else if(PenetrationVector.y > 0) 
                                CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                        }
                        
                        if(PenetrationVector.x != 0)
                        {
                            PV.x = PenetrationVector.x * 1.001f; // This is necessary to prevent the player from getting stuck
                        }
                        
                        if(PenetrationVector.y != 0)
                        {
                            PV.y = PenetrationVector.y * 1.001f; // This is necessary to prevent the player from getting stuck
                        }
                        
                        if(Entity->Type == Entity_Barrel)
                        {
                            Entity->Velocity = math::v2(0.0f,0.0f);
                        }
                    }
                }
            }
        }
        
        if(!Entity->CollisionAABB.IsTrigger)
        {
            Entity->Position += PV;
            Entity->CollisionAABB.Center = math::v2(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale + Entity->CollisionAABB.Offset.y);
        }
    }
}