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

void ClosestPointsOnBoundsToPoint(collision_AABB* Coll, glm::vec2 Point, glm::vec2* Out)
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

real32 GetRayIntersectionFractionOfFirstRay(glm::vec2 OriginA, glm::vec2 EndA,glm::vec2 OriginB, glm::vec2 EndB)
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

real32 GetRayIntersectionFraction(collision_AABB* Coll, glm::vec2 Origin, glm::vec2 Direction)
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

void CheckCollision(game_state* GameState, entity* Entity, collision_info* CollisionInfo)
{
    CollisionInfo->OtherCount = -1;
    
    if(!Entity->IsKinematic && Entity->Active)
    {
        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->CollisionAABB.Offset.y);
        
        if(Entity->HitTrigger)
        {
            Entity->HitTrigger->Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->HitTrigger->Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->HitTrigger->Offset.y);
        }
        
        glm::vec2 PV;
        CollisionInfo->OtherCount = 0;
        
        for(uint32 OtherEntityIndex = 0;
            OtherEntityIndex < GameState->EntityCount;
            OtherEntityIndex++)
        {
            entity* OtherEntity = &GameState->Entities[OtherEntityIndex];
            
            if(!(OtherEntity->Layer & Entity->IgnoreLayers) && !(Entity->Layer & OtherEntity->IgnoreLayers) 
               && OtherEntity->EntityIndex != Entity->EntityIndex 
               && !OtherEntity->IsKinematic && OtherEntity->Active)
            {
                if(OtherEntity->HitTrigger)
                {
                    collision_AABB MdHit;
                    MinkowskiDifference(OtherEntity->HitTrigger, &Entity->CollisionAABB, &MdHit);
                    if(MdHit.Min.x <= 0 &&
                       MdHit.Max.x >= 0 &&
                       MdHit.Min.y <= 0 &&
                       MdHit.Max.y >= 0)
                    {
                        CollisionInfo->Other[CollisionInfo->OtherCount++] = OtherEntity;
                        
                        OtherEntity->HitTrigger->IsColliding = true;
                    }
                    else 
                    {
                        OtherEntity->HitTrigger->IsColliding = false;
                    }
                }
                
                collision_AABB Md;
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
                        
                        //calculate what side is colliding
                        auto OtherPosition = OtherEntity->CollisionAABB.Center;
                        auto OtherExtents = OtherEntity->CollisionAABB.Extents;
                        auto Position = Entity->CollisionAABB.Center;
                        auto Extents = Entity->CollisionAABB.Extents;
                        
                        AABBMin(&Md);
                        AABBMax(&Md);
                        AABBSize(&Md);
                        glm::vec2 PenetrationVector;
                        ClosestPointsOnBoundsToPoint(&Md, glm::vec2(0,0), &PenetrationVector);
                        
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
        
        if(Entity->Type == Entity_Player || Entity->Type == Entity_Enemy || Entity->Type == Entity_Barrel)
        {
            level* Level = &GameState->CurrentLevel;
            
            int32 XPos = (int32)(Entity->Position.x + Entity->Center.x * Entity->Scale.x);
            int32 YPos = (int32)(Entity->Position.y + Entity->Center.y * Entity->Scale.y);
            
            //@Improvement Is it necessary to go 2 tiles out?
            int32 MinX = Max(0, XPos - 2);
            int32 MaxX = Max(0, Min((int32)Level->Tilemap.Width, XPos + 2));
            int32 MinY = Max(0, YPos - 2);
            int32 MaxY = Max(0, Min((int32)Level->Tilemap.Height, YPos + 2));
            
            //check tile collision
            for(int32 X = MinX; X < MaxX; X++)
            {
                for(int32 Y = MinY; Y < MaxY; Y++)
                {
                    tile_data Tile = Level->Tilemap.Data[X][Y];
                    
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
                            glm::vec2 PenetrationVector;
                            ClosestPointsOnBoundsToPoint(&Md, glm::vec2(0,0), &PenetrationVector);
                            
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
                                Entity->Velocity = glm::vec2(0.0f,0.0f);
                            }
                        }
                    }
                }
            }
        }
        
        if(!Entity->CollisionAABB.IsTrigger)
        {
            Entity->Position += PV;
            Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->CollisionAABB.Offset.y);
        }
    }
}