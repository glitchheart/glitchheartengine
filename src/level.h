#ifndef LEVEL_H
#define LEVEL_H

enum Tile_Type
{
    Tile_None,
    Tile_Sand, 
    Tile_Grass, 
    Tile_DarkGrass, 
    Tile_Stone
};

enum Tile_Layer //TODO(Daniel) use this for making different tile layers later
{
};

struct tile_data
{
    Tile_Type Type;
    Tile_Layer Layer;
    glm::vec2 TextureOffset;
    bool32 IsSolid;
    glm::vec2 Center = glm::vec2(0.5, 0.5);
    collision_AABB CollisionAABB;
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
    tile_data** Data;
};

#endif

