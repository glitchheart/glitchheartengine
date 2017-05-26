

// This is calculated with some heuristic
static void AStarComputeHCost(astar_node* Node, astar_node TargetNode)
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
            NeighbourNode.IsClosed = false;
            
            NeighbourNode.GCost = Current->GCost + Cost;
            
            AStarComputeHCost(&NeighbourNode,*TargetNode);
            NeighbourNode.FCost = NeighbourNode.GCost + NeighbourNode.HCost;
            
            if(*OpenSetCount < 512) {
                OpenSet[*OpenSetCount] = NeighbourNode;
                *OpenSetCount = *OpenSetCount + 1;
                OpenSet[*OpenSetCount - 1].WorkingListIndex = *WorkingListCount;
                NeighbourNode.WorkingListIndex = *WorkingListCount;
                NeighbourNode.ParentIndex = Current->WorkingListIndex;
                WorkingList[*WorkingListCount] = NeighbourNode;
                *WorkingListCount = *WorkingListCount + 1;
                OpenSet[*OpenSetCount - 1].ParentIndex = Current->WorkingListIndex;
            }
        }
        else 
        {
            astar_node NeighbourNode  = OpenSet[Index];
            uint32 NewGCost = Current->GCost + Cost;
            if(NewGCost < NeighbourNode.GCost)
            {
                NeighbourNode.GCost = NewGCost;
                NeighbourNode.FCost = NewGCost + NeighbourNode.HCost;
                NeighbourNode.ParentIndex = Current->WorkingListIndex;
                
                OpenSet[Index] = NeighbourNode;
                WorkingList[NeighbourNode.WorkingListIndex].GCost = NewGCost;
                WorkingList[NeighbourNode.WorkingListIndex].FCost = NewGCost + NeighbourNode.HCost;
                WorkingList[NeighbourNode.WorkingListIndex].ParentIndex = Current->WorkingListIndex;
            }
        }
    }
}

/*
* FCost = GCost + HCost
* GCost: Cost from starting tile to current tile
* HCost: Heuristic cost from current tile to target. Can be calculated differently
*        but how do we best do this?
*        - Directly: Just calculate horizontally until X reached and then same for 
*          vertical
* Horizontal cost: 10
* Diagonal cost  : 14 (normally sqrt of 2 * 10, but we like ints!)
*/
static void AStar(entity* Enemy, game_state* GameState, glm::vec2 StartPos, glm::vec2 TargetPos)
{
    if(Enemy->AStarPath)
    {
        free(Enemy->AStarPath);
    }
    astar_working_data AStarWorkingData = {};
    tile_data StartTile = GameState->CurrentLevel.Tilemap.Data[(uint32)StartPos.x][(uint32)StartPos.y];
    tile_data TargetTile = GameState->CurrentLevel.Tilemap.Data[(uint32)TargetPos.x][(uint32)TargetPos.y];
    glm::vec2 CurrentPos = StartPos;
    
    uint32 WorkingListCount = 0;
    uint32 ClosedSetCount = 0;
    astar_node StartNode = {};
    StartNode.X = (uint32)StartPos.x;
    StartNode.Y = (uint32)StartPos.y;
    
    astar_node TargetNode = {};
    TargetNode.X = (uint32)TargetPos.x;
    TargetNode.Y = (uint32)TargetPos.y;
    
    uint32 OpenSetCount = 0;
    StartNode.WorkingListIndex = 0;
    AStarWorkingData.WorkingList[WorkingListCount++] = StartNode;
    AStarWorkingData.WorkingList[WorkingListCount++] = TargetNode;
    TargetNode.WorkingListIndex = 1;
    AStarWorkingData.OpenSet[OpenSetCount++] = StartNode;
    AStarComputeHCost(&StartNode,TargetNode);
    
    int32 WorkingIndex = 0;
    uint32 LowestFcost = StartNode.FCost;
    
    while(OpenSetCount > 0)
    {
        for(uint32 OpenIndex = 0; OpenIndex < OpenSetCount; OpenIndex++)
        {
            if(AStarWorkingData.OpenSet[OpenIndex].FCost < LowestFcost)
            {
                LowestFcost = AStarWorkingData.OpenSet[OpenIndex].FCost;
                WorkingIndex = OpenIndex;
            }
        }
        
        astar_node LastNode = AStarWorkingData.OpenSet[OpenSetCount - 1];
        astar_node Current = AStarWorkingData.OpenSet[WorkingIndex];
        
        
        if(LastNode.X != AStarWorkingData.OpenSet[WorkingIndex].X || LastNode.Y != AStarWorkingData.OpenSet[WorkingIndex].Y)
        {
            AStarWorkingData.OpenSet[WorkingIndex] = LastNode;
        } // Else AStarWorkingData.OpenSet[WorkingIndex] == LastNode
        
        
        AStarWorkingData.OpenSet[OpenSetCount - 1].X = -1;
        AStarWorkingData.OpenSet[OpenSetCount - 1].Y = -1;
        AStarWorkingData.OpenSet[OpenSetCount - 1].IsClosed = false;
        AStarWorkingData.OpenSet[OpenSetCount - 1].FCost = 0;
        AStarWorkingData.OpenSet[OpenSetCount - 1].GCost = 0;
        AStarWorkingData.OpenSet[OpenSetCount - 1].HCost = 0;
        AStarWorkingData.OpenSet[OpenSetCount - 1].ParentIndex = -1;
        AStarWorkingData.OpenSet[OpenSetCount - 1].WorkingListIndex = -1;
        
        OpenSetCount--;
        AStarWorkingData.ClosedSet[ClosedSetCount++] = Current;
        
        if(Current.X == TargetNode.X && Current.Y == TargetNode.Y)
        {
            astar_node PathNode;
            if(Current.ParentIndex >= 0) {
                PathNode = AStarWorkingData.WorkingList[Current.ParentIndex];
                uint32 Length = 1;
                while(PathNode.ParentIndex >= 0 && (PathNode.X != StartNode.X || PathNode.Y != StartNode.Y))
                {
                    Length++;
                    PathNode = AStarWorkingData.WorkingList[PathNode.ParentIndex];
                }
                
                if(Length > 0) 
                {
                    
                    Enemy->AStarPath = (glm::vec2*)malloc(sizeof(glm::vec2) * Length);
                    Enemy->AStarPath[Length] = glm::vec2(AStarWorkingData.WorkingList[Current.WorkingListIndex].X,AStarWorkingData.WorkingList[Current.WorkingListIndex].Y);
                    uint32 Index = Length - 1;
                    PathNode = AStarWorkingData.WorkingList[Current.ParentIndex];
                    while(PathNode.ParentIndex >= 0 && PathNode.X != StartNode.X && PathNode.Y != StartNode.Y)
                    {
                        Enemy->AStarPath[Index--] = glm::vec2(PathNode.X,PathNode.Y);
                        PathNode = AStarWorkingData.WorkingList[PathNode.ParentIndex];
                    }
                    Enemy->AStarPath[0] = glm::vec2(PathNode.X,PathNode.Y);
                    Enemy->AStarPathLength = Length;
                    Enemy->Enemy.Path = true;
                }
                
            }
            // We're done with the path
            return;
        }
        
        if(Current.X != -1 && Current.Y != -1) {
            if(Current.X > 0 && Current.Y > 0)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X - 1, Current.Y - 1,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,14,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            if(Current.X > 0)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X - 1, Current.Y,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,10,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            if(Current.Y > 0)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X, Current.Y - 1,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,10,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            if(Current.X < (int32)GameState->CurrentLevel.Tilemap.Width - 1)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X + 1, Current.Y,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,10,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            
            if(Current.Y < (int32)GameState->CurrentLevel.Tilemap.Height - 1)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X, Current.Y + 1,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,10,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            if(Current.X < (int32)GameState->CurrentLevel.Tilemap.Width - 1 && Current.Y < (int32)GameState->CurrentLevel.Tilemap.Height - 1)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X + 1, Current.Y + 1,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,14,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            if(Current.X > 0 && Current.Y < (int32)GameState->CurrentLevel.Tilemap.Height - 1)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X - 1, Current.Y + 1,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,14,AStarWorkingData.WorkingList,&WorkingListCount);
            }
            
            
            if(Current.X < (int32)GameState->CurrentLevel.Tilemap.Width - 1 && Current.Y > 0)
            {
                HandleNeighbour(&Current, &TargetNode,GameState,Current.X + 1, Current.Y - 1,AStarWorkingData.OpenSet,&OpenSetCount,AStarWorkingData.ClosedSet,ClosedSetCount,14,AStarWorkingData.WorkingList,&WorkingListCount);
            } 
        }
        
        LowestFcost = 10000000;
        WorkingIndex = -1;
    }
}


