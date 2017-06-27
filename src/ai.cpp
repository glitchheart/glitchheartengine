// This is calculated with some heuristic
// In this case we use diagonal distance (http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html)
static void AStarComputeHCost(astar_node* Node, astar_node& TargetNode)
{
    int32 Dx = Abs(Node->X - TargetNode.X);
    int32 Dy = Abs(Node->Y - TargetNode.Y);
    Node->HCost = 10 * (Dx + Dy) + (14 - 2 * 10) * Min(Dx,Dy);
}

static bool32 IsClosed(int32 X, int32 Y, astar_node ClosedSet[], uint32 ClosedSetCount)
{
    for(uint32 ClosedIndex = 0; ClosedIndex < ClosedSetCount; ClosedIndex++)
    {
        if(X == ClosedSet[ClosedIndex].X && Y == ClosedSet[ClosedIndex].Y && ClosedSet[ClosedIndex].X != -1 && ClosedSet[ClosedIndex].Y != -1)
        {
            
            return true;
        }
    }
    return false;
}

static int32 GetOpen(int32 X, int32 Y,astar_node OpenSet[],uint32 OpenSetCount)
{
    for(uint32 OpenIndex = 0; OpenIndex < OpenSetCount; OpenIndex++)
    {
        if(X == OpenSet[OpenIndex].X && Y == OpenSet[OpenIndex].Y)
        {
            return OpenIndex;
        }
    }
    return -1;
}

static void HandleNeighbour(astar_node* Current, astar_node* TargetNode,game_state* GameState, int32 X, int32 Y, astar_node OpenSet[], uint32* OpenSetCount, astar_node ClosedSet[], uint32 ClosedSetCount, uint32 Cost, astar_node WorkingList[], uint32* WorkingListCount)
{
    tile_data CurrentNeighbour = GameState->CurrentLevel.Tilemap.Data[X][Y];
    if(!CurrentNeighbour.IsSolid && !IsClosed(X, Y,ClosedSet,ClosedSetCount))
    {
        int32 Index = GetOpen(X, Y,OpenSet,*OpenSetCount);
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
            uint32 NewGCost = Current->GCost + Cost;
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
        uint32 Length = 1;
        while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Y != StartNode.Y))
        {
            Length++;
            if(GameState->CurrentLevel.Tilemap.Data[PathNode.X][PathNode.Y + 1].IsSolid || GameState->CurrentLevel.Tilemap.Data[PathNode.X][PathNode.Y - 1].IsSolid)
            {
                Length++;
            }
            
            if(GameState->CurrentLevel.Tilemap.Data[PathNode.X + 1][PathNode.Y].IsSolid || GameState->CurrentLevel.Tilemap.Data[PathNode.X - 1][PathNode.Y].IsSolid)
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
            
            uint32 Index = Length - 1;
            Path->AStarPath[Length] = {Current.X,Current.Y};
            PathNode = AStarWorkingData->WorkingList[Current.ParentIndex];
            while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Y != StartNode.Y))
            {
                Assert(!GameState->CurrentLevel.Tilemap.Data[PathNode.X][PathNode.Y].IsSolid);
                Path->AStarPath[Index--] = {PathNode.X,PathNode.Y};
                astar_node PrevNode = PathNode;
                PathNode = AStarWorkingData->WorkingList[PathNode.ParentIndex];
                
                if(GameState->CurrentLevel.Tilemap.Data[PrevNode.X][PrevNode.Y + 1].IsSolid || GameState->CurrentLevel.Tilemap.Data[PrevNode.X][PrevNode.Y - 1].IsSolid)
                {
                    if(PrevNode.X > PathNode.X)
                        Path->AStarPath[Index--] = {PrevNode.X - 1, PrevNode.Y};
                    if(PrevNode.X < PathNode.X)
                        Path->AStarPath[Index--] = {PrevNode.X + 1, PrevNode.Y};
                }
                
                if(GameState->CurrentLevel.Tilemap.Data[PrevNode.X + 1][PrevNode.Y].IsSolid || GameState->CurrentLevel.Tilemap.Data[PrevNode.X - 1][PrevNode.Y].IsSolid)
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
    int32 StartX = (int32)glm::floor(StartPos.x);
    int32 StartY = (int32)glm::floor(StartPos.y);
    
    int32 TargetX = (int32)glm::floor(TargetPos.x);
    int32 TargetY = (int32)glm::floor(TargetPos.y);
    
    if((uint32)StartX < GameState->CurrentLevel.Tilemap.Width && (uint32)StartY < GameState->CurrentLevel.Tilemap.Height &&
       StartX >= 0 && StartY >= 0 && (uint32)TargetX < GameState->CurrentLevel.Tilemap.Width && (uint32)TargetY < GameState->CurrentLevel.Tilemap.Height &&
       TargetX >= 0 && TargetY >= 0)
    {
        astar_working_data* AStarWorkingData = (astar_working_data*)malloc(sizeof(astar_working_data));
        tile_data StartTile = GameState->CurrentLevel.Tilemap.Data[StartX][StartY];
        tile_data TargetTile = GameState->CurrentLevel.Tilemap.Data[TargetX][TargetY];
        glm::vec2 CurrentPos = StartPos;
        
        uint32 WorkingListCount = 0;
        uint32 ClosedSetCount = 0;
        astar_node StartNode = {};
        StartNode.X = StartX;
        StartNode.Y = StartY;
        
        astar_node TargetNode = {};
        TargetNode.X = TargetX;
        TargetNode.Y = TargetY;
        
        uint32 OpenSetCount = 0;
        StartNode.WorkingListIndex = 0;
        AStarWorkingData->WorkingList[WorkingListCount++] = StartNode;
        AStarWorkingData->WorkingList[WorkingListCount++] = TargetNode;
        TargetNode.WorkingListIndex = 1;
        AStarWorkingData->OpenSet[OpenSetCount++] = StartNode;
        AStarComputeHCost(&StartNode,TargetNode);
        
        int32 WorkingIndex = 0;
        uint32 LowestFcost = StartNode.FCost;
        
        while(OpenSetCount > 0 && OpenSetCount < OPENSET_COUNT - 8 && 
              WorkingListCount < WORKING_LIST_COUNT - 8)
        {
            for(uint32 OpenIndex = 0; OpenIndex < OpenSetCount; OpenIndex++)
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
            
            
            
            if(Current.X != -1 && Current.Y != -1 && Current.X > 0 && Current.Y > 0 && Current.X < (int32)GameState->CurrentLevel.Tilemap.Width - 1 && Current.Y < (int32)GameState->CurrentLevel.Tilemap.Height - 1)
            {
                
                for(int32 X = Current.X - 1; X < Current.X + 2; X++)
                {
                    for(int32 Y = Current.Y - 1; Y < Current.Y + 2; Y++)
                    {
                        if(X == Current.X && Y == Current.Y)
                            continue;
                        
                        int32 Cost = 10;
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
    real64 DistanceToTargetEntity = abs(glm::distance(Entity->Position, TargetEntity.Position));
    glm::vec2 EntityPosition = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x,Entity->Position.y + Entity->Center.y * Entity->Scale.y);
    if(TimerDone(GameState,Path->AStarCooldownTimer) || !Path->AStarPath || (Path->AStarPathLength <= Path->PathIndex && DistanceToTargetEntity >= 3.0f)) 
    {
        Path->PathIndex = 0;
        StartTimer(GameState, Path->AStarCooldownTimer);
        glm::vec2 StartPosition = EntityPosition;
        glm::vec2 TargetPosition = glm::vec2(TargetEntity.Position.x + TargetEntity.Center.x * TargetEntity.Scale.x,
                                             TargetEntity.Position.y + TargetEntity.Center.y * TargetEntity.Scale.y);
        AStar(Entity,GameState,StartPosition,TargetPosition);
    }
    
}

static void FollowPath(game_state* GameState, entity* Entity,entity& TargetEntity, real64 DeltaTime, astar_path* Path)
{
    real64 DistanceToTargetEntity = abs(glm::distance(Entity->Position, TargetEntity.Position));
    glm::vec2 EntityPosition = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x,Entity->Position.y + Entity->Center.y * Entity->Scale.y);
    if(Path->AStarPath && Path->PathIndex < Path->AStarPathLength)
    {
        path_node NewPos = Path->AStarPath[Path->PathIndex];
        //printf("NewPos: (%d,%d)\n",NewPos.X,NewPos.Y);
        real64 DistanceToNode = glm::distance(EntityPosition, glm::vec2(NewPos.X,NewPos.Y));
        if(DistanceToNode > 0.8f) 
        {
            glm::vec2 FollowDirection = glm::vec2(NewPos.X,NewPos.Y) - EntityPosition;
            FollowDirection = glm::normalize(FollowDirection);
            
            // NOTE(Niels): Remember 5.0 is only temp, needs actual walking speed!!
            Entity->Velocity = glm::vec2(FollowDirection.x * 5.0f, FollowDirection.y * 5.0f);
        }
        else
        {
            Path->PathIndex++;
        }
    }
    
    glm::vec2 Direction = TargetEntity.Position - Entity->Position;
    // NOTE(Niels): Get actual min distance here (instead of 2)!!
    if(DistanceToTargetEntity > 2 && DistanceToTargetEntity < 3.0f)
    {
        // NOTE(Niels): Remember 5.0 is only temp, needs actual walking speed!!
        Direction = glm::normalize(Direction);
        Entity->Velocity =glm::vec2(Direction.x * 5.0f,
                                    Direction.y * 5.0f);
    }
    Entity->IsFlipped = Direction.x < 0;
}