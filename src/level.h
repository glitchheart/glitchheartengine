#ifndef LEVEL_H
#define LEVEL_H

enum Tile_Type
{
    Tile_None,
    Tile_Sand, 
    Tile_Grass, 
    Tile_DarkGrass, 
    Tile_Stone,
    
    Tile_Count
};

enum Tile_Layer //TODO(Daniel) use this for making different tile layers later
{
    Tile_Layer_None
};

struct tile_data
{
    Tile_Type Type;
    glm::vec2 TextureOffset;
    bool32 IsSolid;
    glm::vec2 Center;
    collision_AABB CollisionAABB;
    Tile_Layer Layer;
};

struct room
{
    real32 X;
    real32 Y;
    uint32 Width;
    uint32 Height;
    render_entity RenderEntity;
    tile_data** Data;
};


struct tilemap
{
    uint32 Width;
    uint32 Height;
    render_entity RenderEntity;
    
    tile_data Tiles[3] = {
        {Tile_Grass,
            glm::vec2(0,0), 
            false,
            glm::vec2(0.5f,0.5f),
            {}},
        {Tile_Stone,
            glm::vec2(0.8f,0.0f),
            true,
            glm::vec2(0.5f,0.5f),
            {}},
        {Tile_Sand,
            glm::vec2(0.6f,0.0f),
            false,
            glm::vec2(0.5f,0.5f),
            {}}
    };
    
    tile_data** Data;
};

struct level
{
    char* Name;
    glm::vec2 PlayerStartPosition;
    tilemap Tilemap;
};


#endif

