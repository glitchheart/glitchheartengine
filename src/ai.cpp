// This is calculated with some heuristic
// In this case we use diagonal distance (http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html)
static void AStarComputeHCost(astar_node* Node, astar_node& TargetNode)
{
    i32 Dx = Abs(Node->X - TargetNode.X);
    i32 Dy = Abs(Node->Y - TargetNode.Y);
    Node->HCost = 10 * (Dx + Dy) + (14 - 2 * 10) * Min(Dx,Dy);
}

static b32 IsClosed(i32 X, i32 Y, astar_node ClosedSet[], u32 ClosedSetCount)
{
    for(u32 ClosedIndex = 0; ClosedIndex < ClosedSetCount; ClosedIndex++)
    {
        if(X == ClosedSet[ClosedIndex].X && Y == ClosedSet[ClosedIndex].Y && ClosedSet[ClosedIndex].X != -1 && ClosedSet[ClosedIndex].Y != -1)
        {
            
            return true;
        }
    }
    return false;
}

static i32 GetOpen(i32 X, i32 Y,astar_node OpenSet[],u32 OpenSetCount)
{
    for(u32 OpenIndex = 0; OpenIndex < OpenSetCount; OpenIndex++)
    {
        if(X == OpenSet[OpenIndex].X && Y == OpenSet[OpenIndex].Y)
        {
            return OpenIndex;
        }
    }
    return -1;
}

static void HandleNeighbour(astar_node* Current, astar_node* TargetNode,game_state* GameState, i32 X, i32 Y, astar_node OpenSet[], u32* OpenSetCount, astar_node ClosedSet[], u32 ClosedSetCount, u32 Cost, astar_node WorkingList[], u32* WorkingListCount)
{
    tile_data CurrentNeighbour = GameState->CurrentLevel.Tilemap.Data[1][X][Y];
    if(!CurrentNeighbour.IsSolid && !IsClosed(X, Y,ClosedSet,ClosedSetCount))
    {
        i32 Index = GetOpen(X, Y,OpenSet,*OpenSetCount);
        if(Index < 0)
        {
            astar_node NeighbourNode  = {};
            NeighbourNode.X = X;
            NeighbourNode.Y = Y;
            
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
        while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Y != StartNode.Y))
        {
            Length++;
            if(GameState->CurrentLevel.Tilemap.Data[1][PathNode.X][PathNode.Y + 1].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PathNode.X][PathNode.Y - 1].IsSolid)
            {
                Length++;
            }
            
            if(GameState->CurrentLevel.Tilemap.Data[1][PathNode.X + 1][PathNode.Y].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PathNode.X - 1][PathNode.Y].IsSolid)
            {
                Length++;
            }
            
            PathNode = AStarWorkingData->WorkingList[PathNode.ParentIndex];
        }
        
        if(Length > 0) 
        {
            if(Path->AStarPath)
            {
                free(Path->AStarPath);
            }
            
            Path->AStarPath = (path_node*)malloc(sizeof(path_node) * Length + 1);
            
            u32 Index = Length - 1;
            Path->AStarPath[Length] = {Current.X,Current.Y};
            PathNode = AStarWorkingData->WorkingList[Current.ParentIndex];
            
            while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Y != StartNode.Y))
            {
                Assert(!GameState->CurrentLevel.Tilemap.Data[1][PathNode.X][PathNode.Y].IsSolid);
                Path->AStarPath[Index--] = {PathNode.X,PathNode.Y};
                astar_node PrevNode = PathNode;
                PathNode = AStarWorkingData->WorkingList[PathNode.ParentIndex];
                
                if(GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X][PrevNode.Y + 1].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X][PrevNode.Y - 1].IsSolid)
                {
                    if(PrevNode.X > PathNode.X)
                        Path->AStarPath[Index--] = {PrevNode.X - 1, PrevNode.Y};
                    if(PrevNode.X < PathNode.X)
                        Path->AStarPath[Index--] = {PrevNode.X + 1, PrevNode.Y};
                }
                
                if(GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X + 1][PrevNode.Y].IsSolid || GameState->CurrentLevel.Tilemap.Data[1][PrevNode.X - 1][PrevNode.Y].IsSolid)
                {
                    if(PrevNode.Y > PathNode.Y)
                        Path->AStarPath[Index--] = {PrevNode.X, PrevNode.Y - 1};
                    if(PrevNode.Y < PathNode.Y)
                        Path->AStarPath[Index--] = {PrevNode.X, PrevNode.Y + 1};
                }
            }
            //Assert(!GameState->CurrentLevel.Tilemap.Data[PathNode.X][PathNode.Y].IsSolid);
            Path->AStarPath[0] = {PathNode.X,PathNode.Y};
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
static void AStar(entity* Entity, game_state* GameState, glm::vec2 StartPos, glm::vec2 TargetPos)
{
    i32 StartX = (i32)glm::floor(StartPos.x);
    i32 StartY = (i32)glm::floor(StartPos.y);
    
    i32 TargetX = (i32)glm::floor(TargetPos.x);
    i32 TargetY = (i32)glm::floor(TargetPos.y);
    
    if((u32)StartX < GameState->CurrentLevel.Tilemap.Width && (u32)StartY < GameState->CurrentLevel.Tilemap.Height &&
       StartX >= 0 && StartY >= 0 && (u32)TargetX < GameState->CurrentLevel.Tilemap.Width && (u32)TargetY < GameState->CurrentLevel.Tilemap.Height &&
       TargetX >= 0 && TargetY >= 0)
    {
        astar_working_data* AStarWorkingData = (astar_working_data*)malloc(sizeof(astar_working_data));
        tile_data StartTile = GameState->CurrentLevel.Tilemap.Data[1][StartX][StartY];
        tile_data TargetTile = GameState->CurrentLevel.Tilemap.Data[1][TargetX][TargetY];
        glm::vec2 CurrentPos = StartPos;
        
        u32 WorkingListCount = 0;
        u32 ClosedSetCount = 0;
        astar_node StartNode = {};
        StartNode.X = StartX;
        StartNode.Y = StartY;
        
        astar_node TargetNode = {};
        TargetNode.X = TargetX;
        TargetNode.Y = TargetY;
        
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
            
            if(Current.X == TargetNode.X && Current.Y == TargetNode.Y)
            {
                ReconstructPath(GetAStarPath(Entity),GameState,Current,AStarWorkingData, StartNode);
                
                // We're done with the path
                free(AStarWorkingData);
                return;
            }
            
            if(LastNode.X != AStarWorkingData->OpenSet[WorkingIndex].X || LastNode.Y != AStarWorkingData->OpenSet[WorkingIndex].Y)
            {
                AStarWorkingData->OpenSet[WorkingIndex] = LastNode;
            } // Else AStarWorkingData->OpenSet[WorkingIndex] == LastNode
            
            
            AStarWorkingData->OpenSet[OpenSetCount - 1].X = -1;
            AStarWorkingData->OpenSet[OpenSetCount - 1].Y = -1;
            AStarWorkingData->OpenSet[OpenSetCount - 1].FCost = 0;
            AStarWorkingData->OpenSet[OpenSetCount - 1].GCost = 0;
            AStarWorkingData->OpenSet[OpenSetCount - 1].HCost = 0;
            AStarWorkingData->OpenSet[OpenSetCount - 1].ParentIndex = -1;
            AStarWorkingData->OpenSet[OpenSetCount - 1].WorkingListIndex = -1;
            
            OpenSetCount--;
            AStarWorkingData->ClosedSet[ClosedSetCount++] = Current;
            
            if(Current.X != -1 && Current.Y != -1 && Current.X > 0 && Current.Y > 0 && Current.X < (i32)GameState->CurrentLevel.Tilemap.Width - 1 && Current.Y < (i32)GameState->CurrentLevel.Tilemap.Height - 1)
            {
                
                for(i32 X = Current.X - 1; X < Current.X + 2; X++)
                {
                    for(i32 Y = Current.Y - 1; Y < Current.Y + 2; Y++)
                    {
                        if(X == Current.X && Y == Current.Y)
                            continue;
                        
                        i32 Cost = 10;
                        if(Current.X != X && Current.Y != Y)
                        {
                            Cost = 14;
                        }
                        HandleNeighbour(&Current, &TargetNode,GameState,X, Y,
                                        AStarWorkingData->OpenSet,&OpenSetCount,
                                        AStarWorkingData->ClosedSet,ClosedSetCount,Cost,AStarWorkingData->WorkingList,&WorkingListCount);
                        
                    }
                }
            }
            
            LowestFcost = 10000000;
            WorkingIndex = -1;
        }
        free(AStarWorkingData);
    }
}

static void FindPath(game_state* GameState, entity* Entity, entity& TargetEntity,astar_path* Path)
{
    r64 DistanceToTargetEntity = abs(glm::distance(Entity->Position, TargetEntity.Position));
    glm::vec2 EntityPosition = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale,Entity->Position.y + Entity->Center.y * Entity->Scale);
    if(TimerDone(GameState,Path->AStarCooldownTimer) || !Path->AStarPath || (Path->AStarPathLength <= Path->PathIndex && DistanceToTargetEntity >= 3.0f)) 
    {
        Path->PathIndex = 0;
        StartTimer(GameState, Path->AStarCooldownTimer);
        glm::vec2 StartPosition = EntityPosition;
        glm::vec2 TargetPosition = glm::vec2(TargetEntity.Position.x + TargetEntity.Center.x * TargetEntity.Scale,
                                             TargetEntity.Position.y + TargetEntity.Center.y * TargetEntity.Scale);
        AStar(Entity,GameState,StartPosition,TargetPosition);
    }
    
}

static void FollowPath(game_state* GameState, entity* Entity,entity& TargetEntity, r64 DeltaTime, astar_path* Path)
{
    r64 DistanceToTargetEntity = abs(glm::distance(Entity->Position, TargetEntity.Position));
    glm::vec2 EntityPosition = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale, Entity->Position.y + Entity->Center.y * Entity->Scale);
    if(Path->AStarPath && Path->PathIndex < Path->AStarPathLength)
    {
        path_node NewPos = Path->AStarPath[Path->PathIndex];
        
        r64 DistanceToNode = glm::distance(EntityPosition, glm::vec2(NewPos.X,NewPos.Y));
        if(DistanceToNode > 0.8f) 
        {
            glm::vec2 FollowDirection = glm::vec2(NewPos.X,NewPos.Y) - EntityPosition;
            FollowDirection = glm::normalize(FollowDirection);
            
            Entity->Velocity = glm::vec2(FollowDirection.x * Entity->Enemy.WalkingSpeed, FollowDirection.y * Entity->Enemy.WalkingSpeed);
        }
        else
        {
            Path->PathIndex++;
        }
    }
    
    glm::vec2 Direction = TargetEntity.Position - Entity->Position;
    // NOTE(Niels): Get actual min distance here (instead of 2)!!
    
}