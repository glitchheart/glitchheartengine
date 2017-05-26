
struct astar_node
{
    int32 X = -1;
    int32 Y = -1;
    int32 WorkingListIndex = -1;
    bool32 IsClosed = false;
    uint32 FCost = 0;
    uint32 GCost = 0;
    uint32 HCost = 0;
    
    int32 ParentIndex = -1;
};

struct astar_working_data
{
    astar_node OpenSet[96];
    astar_node ClosedSet[96];
    astar_node WorkingList[256];
};





