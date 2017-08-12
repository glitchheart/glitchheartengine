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
    Coll->Size = math::v3(Coll->Extents.x * 2, Coll->Extents.y * 2, Coll->Extents.z * 2);
}

static void MinkowskiDifference(collision_AABB* Coll, collision_AABB* Other, collision_AABB* Out)
{
    AABBMin(Coll);
    AABBMax(Coll);
    AABBSize(Coll);
    AABBMin(Other);
    AABBMax(Other);
    AABBSize(Other);
    math::v3 TopLeft = Coll->Min - Other->Max;
    math::v3 FullSize = Coll->Size + Other->Size;
    math::v3 Center = TopLeft + (FullSize / 2);
    math::v3 Extents = FullSize / 2;
    Out->Center = Center;
    Out->Extents = Extents;
    AABBMin(Out);
    AABBMax(Out);
    AABBSize(Out);
}

static void ClosestPointsOnBoundsToPoint(collision_AABB* Coll, math::v3 Point, math::v3* Out)
{  
    r32 MinDist = Abs(Point.x - Coll->Min.x);
    
    *Out = math::v3(Coll->Min.x, 0.0f, Point.z);
    
    if(Abs(Coll->Max.x - Point.x) < MinDist)
    {
        MinDist = Abs(Coll->Max.x - Point.x);
        *Out = math::v3(Coll->Max.x, 0.0f, Point.z);
    }
    
    if(Abs(Coll->Max.z - Point.z) < MinDist)
    {
        MinDist = Abs(Coll->Max.z - Point.z);
        *Out = math::v3(Point.x, 0.0f, Coll->Max.z);
    }
    if(Abs(Coll->Min.z - Point.z) < MinDist)
    {
        MinDist = Abs(Coll->Min.z - Point.z);
        *Out = math::v3(Point.x, 0.0f, Coll->Min.z);
    }
}

static r32 GetRayIntersectionFractionOfFirstRay(math::v3 OriginA, math::v3 EndA,math::v3 OriginB, math::v3 EndB)
{
    math::v3 R = EndA - OriginA;
    math::v3 S = EndB - OriginB;
    
    r32 Numerator = math::Dot((OriginB - OriginA),  R);
    r32 Denominator = math::Dot(R , S);
    
    if(Numerator == 0 && Denominator == 0)
    {
        return INFINITY;
    }
    
    if(Denominator == 0)
    {
        return INFINITY;
    }
    
    r32 U = Numerator / Denominator;
    r32 T = ((math::Dot(OriginA / OriginB,S))) / Denominator;
    
    if ((T >= 0) && (T <= 1) && (U >= 0) && (U <= 1))
    {
        return T;
    }
    
    return INFINITY;
}

r32 GetRayIntersectionFraction(collision_AABB* Coll, math::v3 Origin, math::v3 Direction)
{
    AABBMin(Coll);
    AABBMax(Coll);
    math::v3 End = Origin + Direction;
    
    r32 MinT = GetRayIntersectionFractionOfFirstRay(Origin,End,math::v3(Coll->Min.x, 0.0f, Coll->Min.z),
                                                    math::v3(Coll->Min.x, 0.0f, Coll->Max.z));
    r32 X;
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, math::v3(Coll->Min.x, 0.0f, Coll->Max.z),
                                             math::v3(Coll->Max.x, 0.0f, Coll->Max.z));
    if(X < MinT)
    {
        MinT = X;
    }
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, math::v3(Coll->Max.x, 0.0f, Coll->Max.z),
                                             math::v3(Coll->Max.x, 0.0f, Coll->Min.z));
    if(X < MinT)
    {
        MinT = X;
    }
    X = GetRayIntersectionFractionOfFirstRay(Origin,End, math::v3(Coll->Max.x, 0.0f, Coll->Min.z),
                                             math::v3(Coll->Min.x, 0.0f, Coll->Min.z));
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
                   MdHit.Min.z <= 0 &&
                   MdHit.Max.z >= 0)
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
        Entity->CollisionAABB.Center = math::v3(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->CollisionAABB.Offset.x, 0.0f, Entity->Position.z + Entity->Center.z * Entity->Scale + Entity->CollisionAABB.Offset.z);
        
        if(Entity->Type == Entity_Enemy)
        {
            Entity->Enemy.EnemyCollider.Center = math::v3(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->Enemy.EnemyCollider.Offset.x, 0.0f, Entity->Position.z + Entity->Center.z * Entity->Scale + Entity->Enemy.EnemyCollider.Offset.z);
        }
        
        if(Entity->HasHitTrigger)
        {
            Entity->HitTrigger.Center = math::v3(Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->HitTrigger.Offset.x, 0.0f, Entity->Position.z + Entity->Center.z * Entity->Scale + Entity->HitTrigger.Offset.z);
        }
        
        math::v3 PV;
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
                       MdHit.Min.z <= 0 &&
                       MdHit.Max.z >= 0)
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
                   Md.Min.z <= 0 &&
                   Md.Max.z >= 0)
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
                        math::v3 PenetrationVector;
                        ClosestPointsOnBoundsToPoint(&Md, math::v3(0, 0, 0), &PenetrationVector);
                        
                        if(Abs(PenetrationVector.x) > Abs(PenetrationVector.z))
                        {
                            if(PenetrationVector.x > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                            else if(PenetrationVector.x < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                        }
                        else
                        {
                            if(PenetrationVector.z > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                            else if(PenetrationVector.z < 0) 
                                CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                        }
                        
                        if(PenetrationVector.x != 0)
                        {
                            PV.x = PenetrationVector.x;
                        }
                        
                        if(PenetrationVector.z != 0)
                        {
                            PV.z = PenetrationVector.z;
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
        i32 ZPos = (i32)(Entity->Position.z + Entity->Center.z * Entity->Scale);
        
        //@Improvement Is it necessary to go 2 tiles out?
        i32 MinX = Max(0, XPos - 2);
        i32 MaxX = Max(0, Min((i32)Level->Tilemap.Width, XPos + 2));
        i32 MinZ = Max(0, ZPos - 2);
        i32 MaxZ = Max(0, Min((i32)Level->Tilemap.Height, ZPos + 2));
        
        //check tile collision
        for(i32 X = MinX; X < MaxX; X++)
        {
            for(i32 Z = MinZ; Z < MaxZ; Z++)
            {
                tile_data Tile = Level->Tilemap.Data[1][X][Z];
                
                if(Tile.IsSolid)
                {
                    //@Cleanup we have to move this to a separate function, because it is used in entity collision and tile collision
                    collision_AABB Md;
                    MinkowskiDifference(&Tile.CollisionAABB, &Entity->CollisionAABB, &Md);
                    if(Md.Min.x <= 0 &&
                       Md.Max.x >= 0 &&
                       Md.Min.z <= 0 &&
                       Md.Max.z >= 0)
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
                        math::v3 PenetrationVector;
                        ClosestPointsOnBoundsToPoint(&Md, math::v3(0,0,0), &PenetrationVector);
                        
                        if(Abs(PenetrationVector.x) > Abs(PenetrationVector.z))
                        {
                            if(PenetrationVector.x > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                            else if(PenetrationVector.x < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                        }
                        else
                        {
                            if(PenetrationVector.z < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                            else if(PenetrationVector.z > 0) 
                                CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                        }
                        
                        if(PenetrationVector.x != 0)
                        {
                            PV.x = PenetrationVector.x * 1.001f; // This is necessary to prevent the player from getting stuck
                        }
                        
                        if(PenetrationVector.z != 0)
                        {
                            PV.z = PenetrationVector.z * 1.001f; // This is necessary to prevent the player from getting stuck
                        }
                        
                        if(Entity->Type == Entity_Barrel)
                        {
                            Entity->Velocity = math::v3(0.0f, 0.0f, 0.0f);
                        }
                    }
                }
            }
        }
        
        if(!Entity->CollisionAABB.IsTrigger)
        {
            Entity->Position += PV;
            Entity->CollisionAABB.Center = math::v3
                (Entity->Position.x + Entity->Center.x * Entity->Scale + Entity->CollisionAABB.Offset.x, 
                 0.0f,
                 Entity->Position.z + Entity->Center.z * Entity->Scale + Entity->CollisionAABB.Offset.z);
        }
    }
}