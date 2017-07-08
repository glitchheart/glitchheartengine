#ifndef AI_H
#define AI_H

struct astar_node
{
    i32 X = -1;
    i32 Y = -1;
    i32 WorkingListIndex = -1;
    u32 FCost = 0;
    u32 GCost = 0;
    u32 HCost = 0;
    
    i32 ParentIndex = -1;
};

struct path_node
{
    i32 X;
    i32 Y;
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
    path_node* AStarPath = 0;
    u32 AStarPathLength = 0;
    u32 PathIndex = 0;
    timer AStarCooldownTimer;
    astar_path(){}
};

#endif