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
    glm::vec2 TextureSize;
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
    uint32 TileSize = 32;
    tilemap_render_info RenderInfo;
    render_entity RenderEntity;
    
    tile_data* Tiles;
    
    tile_data** Data;
};

struct level
{
    char* Name;
    glm::vec2 PlayerStartPosition;
    tilemap Tilemap;
};


#endif

