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

struct tile_data
{
    Tile_Type Type;
    glm::vec2 TextureOffset;
};

#define ROOM_WIDTH 30
#define ROOM_HEIGHT 30

struct room
{
    real32 X;
    real32 Y;
    uint32 Width;
    uint32 Height;
    render_entity RenderEntity;
    tile_data** Data;
};

#endif

