struct astar_node
{
    int32 X = -1;
    int32 Y = -1;
    int32 WorkingListIndex = -1;
    uint32 FCost = 0;
    uint32 GCost = 0;
    uint32 HCost = 0;
    
    int32 ParentIndex = -1;
};

struct path_node
{
    int32 X;
    int32 Y;
};

#define OPENSET_COUNT 512
#define CLOSEDSET_COUNT 512
#define WORKING_LIST_COUNT 1024

struct astar_working_data
{
    astar_node OpenSet[OPENSET_COUNT];
    astar_node ClosedSet[CLOSEDSET_COUNT];
    astar_node WorkingList[WORKING_LIST_COUNT];
};

struct astar_path
{
    path_node* AStarPath;
    uint32 AStarPathLength;
    uint32 PathIndex;
    timer* AStarCooldownTimer;
};




