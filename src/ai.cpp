// This is calculated with some heuristic
// In this case we use diagonal distance (http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html)
static void AStarComputeHCost(astar_node* Node, astar_node& TargetNode)
{
    i32 Dx = Abs(Node->X - TargetNode.X);
    i32 Dz = Abs(Node->Z - TargetNode.Z);
    Node->HCost = 10 * (Dx + Dz) + (14 - 2 * 10) * Min(Dx,Dz);
}

static b32 IsClosed(i32 X, i32 Z, astar_node ClosedSet[], u32 ClosedSetCount)
{
    for(u32 ClosedIndex = 0; ClosedIndex < ClosedSetCount; ClosedIndex++)
    {
        if(X == ClosedSet[ClosedIndex].X && Z == ClosedSet[ClosedIndex].Z && ClosedSet[ClosedIndex].X != -1 && ClosedSet[ClosedIndex].Z != -1)
        {
            
            return true;
        }
    }
    return false;
}

static i32 GetOpen(i32 X, i32 Z,astar_node OpenSet[],u32 OpenSetCount)
{
    for(u32 OpenIndex = 0; OpenIndex < OpenSetCount; OpenIndex++)
    {
        if(X == OpenSet[OpenIndex].X && Z == OpenSet[OpenIndex].Z)
        {
            return OpenIndex;
        }
    }
    return -1;
}

static void HandleNeighbour(astar_node* Current, astar_node* TargetNode,game_state* GameState, i32 X, i32 Z, astar_node OpenSet[], u32* OpenSetCount, astar_node ClosedSet[], u32 ClosedSetCount, u32 Cost, astar_node WorkingList[], u32* WorkingListCount)
{
    tile_data CurrentNeighbour = GameState->CurrentLevel.Tilemap.Data[1][X][Z];
    if(!CurrentNeighbour.IsSolid && !IsClosed(X, Z,ClosedSet,ClosedSetCount))
    {
        i32 Index = GetOpen(X, Z,OpenSet,*OpenSetCount);
        if(Index < 0)
        {
            astar_node NeighbourNode  = {};
            NeighbourNode.X = X;
            NeighbourNode.Z = Z;
            
            NeighbourNode.GCost = Current->GCost + Cost;
            AStarComputeHCost(&NeighbourNode,*TargetNode);
            NeighbourNode.FCost = NeighbourNode.GCost + NeighbourNode.HCost;
            NeighbourNode.WorkingListIndex = *WorkingListCount;
            NeighbourNode.ParentIndex = Current->WorkingListIndex;
            
            if(*OpenSetCount < OPENSET_COUNT) 
            {
                OpenSet[*OpenSetCount] = NeighbourNode;
                *OpenSetCount = *OpenSetCount + 1;
                WorkingList[*WorkingListCount] = NeighbourNode;
                *WorkingListCount = *WorkingListCount + 1;
            }
        }
        else 
        {
            astar_node NeighbourNode  = OpenSet[Index];
            u32 NewGCost = Current->GCost + Cost;
            if(NewGCost < NeighbourNode.GCost)
            {
                OpenSet[Index].GCost = NewGCost;
                OpenSet[Index].FCost = NewGCost + NeighbourNode.HCost;
                OpenSet[Index].ParentIndex = Current->WorkingListIndex;
                
                WorkingList[NeighbourNode.WorkingListIndex].GCost = NewGCost;
                WorkingList[NeighbourNode.WorkingListIndex].FCost = NewGCost + NeighbourNode.HCost;
                WorkingList[NeighbourNode.WorkingListIndex].ParentIndex = Current->WorkingListIndex;
            }
        }
    }
}

static void ReconstructPath(astar_path* Path, game_state* GameState, astar_node& Current,astar_working_data* AStarWorkingData,astar_node& StartNode)
{
    astar_node PathNode;
    
    if(Current.ParentIndex >= 0)
    {
        PathNode = AStarWorkingData->WorkingList[Current.ParentIndex];
        u32 Length = 1;
        while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Z != StartNode.Z))
        {
            Length++;
            if(GameState->CurrentLevel.Tilemap.Data[1][PathNode.X][PathNode.Z + 1].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PathNode.X][PathNode.Z - 1].IsSolid)
            {
                Length++;
            }
            
            if(GameState->CurrentLevel.Tilemap.Data[1][PathNode.X + 1][PathNode.Z].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PathNode.X - 1][PathNode.Z].IsSolid)
            {
                Length++;
            }
            
            PathNode = AStarWorkingData->WorkingList[PathNode.ParentIndex];
        }
        
        if(Length > 0) 
        {
            Assert(Length < MAX_PATH_LENGTH);
            u32 Index = Length - 1;
            Path->AStarPath[Length] = {Current.X,Current.Z};
            PathNode = AStarWorkingData->WorkingList[Current.ParentIndex];
            
            while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Z != StartNode.Z))
            {
                Assert(!GameState->CurrentLevel.Tilemap.Data[1][PathNode.X][PathNode.Z].IsSolid);
                Path->AStarPath[Index--] = {PathNode.X,PathNode.Z};
                astar_node PrevNode = PathNode;
                PathNode = AStarWorkingData->WorkingList[PathNode.ParentIndex];
                
                if(GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X][PrevNode.Z + 1].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X][PrevNode.Z - 1].IsSolid)
                {
                    if(PrevNode.X > PathNode.X)
                        Path->AStarPath[Index--] = {PrevNode.X - 1, PrevNode.Z};
                    if(PrevNode.X < PathNode.X)
                        Path->AStarPath[Index--] = {PrevNode.X + 1, PrevNode.Z};
                }
                
                if(GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X + 1][PrevNode.Z].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X - 1][PrevNode.Z].IsSolid)
                {
                    if(PrevNode.Z > PathNode.Z)
                        Path->AStarPath[Index--] = {PrevNode.X, PrevNode.Z - 1};
                    if(PrevNode.Z < PathNode.Z)
                        Path->AStarPath[Index--] = {PrevNode.X, PrevNode.Z + 1};
                }
            }
            //Assert(!GameState->CurrentLevel.Tilemap.Data[PathNode.X][PathNode.Y].IsSolid);
            Path->AStarPath[0] = {PathNode.X,PathNode.Z};
            Path->AStarPathLength = Length;
            
            Path->PathIndex = 0;
        }
    }
}

static astar_path* GetAStarPath(entity* Entity)
{
    return &Entity->Enemy.AStarPath;
}


/*
* FCost = GCost + HCost
* GCost: Cost from starting tile to current tile
* HCost: Heuristic cost from current tile to target. Can be calculated differently
*        but how do we best do this?
*        -  In this case we use diagonal distance (http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html)
* Horizontal cost: 10
* Diagonal cost  : 14 (normally sqrt of 2 * 10, but we like ints!)
*/
static void AStar(entity* Entity, game_state* GameState, math::v3 StartPos, math::v3 TargetPos)
{
    i32 StartX = (i32)floor(StartPos.x);
    i32 StartZ = (i32)floor(StartPos.z);
    
    i32 TargetX = (i32)floor(TargetPos.x);
    i32 TargetZ = (i32)floor(TargetPos.z);
    
    if(StartX < GameState->CurrentLevel.Tilemap.Width && StartZ < GameState->CurrentLevel.Tilemap.Height &&
       StartX >= 0 && StartZ >= 0 && TargetX < GameState->CurrentLevel.Tilemap.Width && TargetZ < GameState->CurrentLevel.Tilemap.Height &&
       TargetX >= 0 && TargetZ >= 0)
    {
        astar_working_data Data = {};
        astar_working_data* AStarWorkingData = &Data; 
        
        tile_data StartTile = GameState->CurrentLevel.Tilemap.Data[1][StartX][StartZ];
        tile_data TargetTile = GameState->CurrentLevel.Tilemap.Data[1][TargetX][TargetZ];
        math::v3 CurrentPos = StartPos;
        
        u32 WorkingListCount = 0;
        u32 ClosedSetCount = 0;
        astar_node StartNode = {};
        StartNode.X = StartX;
        StartNode.Z = StartZ;
        
        astar_node TargetNode = {};
        TargetNode.X = TargetX;
        TargetNode.Z = TargetZ;
        
        u32 OpenSetCount = 0;
        StartNode.WorkingListIndex = 0;
        AStarWorkingData->WorkingList[WorkingListCount++] = StartNode;
        AStarWorkingData->WorkingList[WorkingListCount++] = TargetNode;
        TargetNode.WorkingListIndex = 1;
        AStarWorkingData->OpenSet[OpenSetCount++] = StartNode;
        AStarComputeHCost(&StartNode,TargetNode);
        
        i32 WorkingIndex = 0;
        u32 LowestFcost = StartNode.FCost;
        
        while(OpenSetCount > 0 && OpenSetCount < OPENSET_COUNT - 8 && 
              WorkingListCount < WORKING_LIST_COUNT - 8)
        {
            for(u32 OpenIndex = 0; OpenIndex < OpenSetCount; OpenIndex++)
            {
                
                if(AStarWorkingData->OpenSet[OpenIndex].FCost < LowestFcost)
                {
                    LowestFcost = AStarWorkingData->OpenSet[OpenIndex].FCost;
                    WorkingIndex = OpenIndex;
                }
            }
            
            astar_node LastNode = AStarWorkingData->OpenSet[OpenSetCount - 1];
            astar_node Current = AStarWorkingData->OpenSet[WorkingIndex];
            
            if(Current.X == TargetNode.X && Current.Z == TargetNode.Z)
            {
                ReconstructPath(GetAStarPath(Entity),GameState,Current,AStarWorkingData, StartNode);
                
                // We're done with the path
                return;
            }
            
            if(LastNode.X != AStarWorkingData->OpenSet[WorkingIndex].X || LastNode.Z != AStarWorkingData->OpenSet[WorkingIndex].Z)
            {
                AStarWorkingData->OpenSet[WorkingIndex] = LastNode;
            } // Else AStarWorkingData->OpenSet[WorkingIndex] == LastNode
            
            
            AStarWorkingData->OpenSet[OpenSetCount - 1].X = -1;
            AStarWorkingData->OpenSet[OpenSetCount - 1].Z = -1;
            AStarWorkingData->OpenSet[OpenSetCount - 1].FCost = 0;
            AStarWorkingData->OpenSet[OpenSetCount - 1].GCost = 0;
            AStarWorkingData->OpenSet[OpenSetCount - 1].HCost = 0;
            AStarWorkingData->OpenSet[OpenSetCount - 1].ParentIndex = -1;
            AStarWorkingData->OpenSet[OpenSetCount - 1].WorkingListIndex = -1;
            
            OpenSetCount--;
            AStarWorkingData->ClosedSet[ClosedSetCount++] = Current;
            
            if(Current.X != -1 && Current.Z != -1 && Current.X > 0 && Current.Z > 0 && Current.X < (i32)GameState->CurrentLevel.Tilemap.Width - 1 && Current.Z < (i32)GameState->CurrentLevel.Tilemap.Height - 1)
            {
                
                for(i32 X = Current.X - 1; X < Current.X + 2; X++)
                {
                    for(i32 Z = Current.Z - 1; Z < Current.Z + 2; Z++)
                    {
                        if(X == Current.X && Z == Current.Z)
                            continue;
                        
                        i32 Cost = 10;
                        if(Current.X != X && Current.Z != Z)
                        {
                            Cost = 14;
                        }
                        HandleNeighbour(&Current, &TargetNode,GameState,X, Z,
                                        AStarWorkingData->OpenSet,&OpenSetCount,
                                        AStarWorkingData->ClosedSet,ClosedSetCount,Cost,AStarWorkingData->WorkingList,&WorkingListCount);
                        
                    }
                }
            }
            
            LowestFcost = 10000000;
            WorkingIndex = -1;
        }
    }
}

void FindPath(game_state* GameState, entity* Entity, entity& TargetEntity,astar_path* Path)
{
    r64 DistanceToTargetEntity = abs(math::Distance(Entity->Position, TargetEntity.Position));
    math::v3 EntityPosition = math::v3(Entity->Position.x + Entity->Center.x * Entity->Scale, Entity->Position.y + Entity->Center.y * Entity->Scale,Entity->Position.z + Entity->Center.z * Entity->Scale);
    
    if(TimerDone(GameState, Path->AStarCooldownTimer) || !Path->AStarPath || (Path->AStarPathLength <= Path->PathIndex && DistanceToTargetEntity >= 3.0f)) 
    {
        Path->PathIndex = 0;
        StartTimer(GameState, Path->AStarCooldownTimer);
        math::v3 StartPosition = EntityPosition;
        math::v3 TargetPosition = math::v3(TargetEntity.Position.x + TargetEntity.Center.x * TargetEntity.Scale, TargetEntity.Position.y + TargetEntity.Center.y * TargetEntity.Scale,
                                           TargetEntity.Position.z + TargetEntity.Center.z * TargetEntity.Scale);
        AStar(Entity,GameState,StartPosition,TargetPosition);
    }
    
}

void FollowPath(game_state* GameState, entity* Entity,entity& TargetEntity,  astar_path* Path)
{
    math::v3 EntityPosition = Entity->Position;
    
    if(Path->AStarPath && Path->PathIndex < Path->AStarPathLength)
    {
        path_node NewPos = Path->AStarPath[Path->PathIndex];
        
        r64 DistanceToNode = math::Distance(EntityPosition, math::v3(NewPos.X, 0.0f, NewPos.Z));
        
        if(DistanceToNode > 0.1f) 
        {
            math::v3 FollowDirection = math::v3(NewPos.X, Entity->Position.y, NewPos.Z) - EntityPosition;
            FollowDirection = math::Normalize(FollowDirection);
            
            Entity->Velocity = math::v3(FollowDirection.x * Entity->Enemy.WalkingSpeed,
                                        Entity->Position.y,
                                        FollowDirection.z * Entity->Enemy.WalkingSpeed);
        }
        else
        {
            Path->PathIndex++;
        }
    }
}