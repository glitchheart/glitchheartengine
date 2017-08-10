#ifndef LEVEL_H
#define LEVEL_H

enum Tile_Layer 
{
    Tile_Layer_Background,
    Tile_Layer_Normal
};

struct tile_data
{
    i32 TypeIndex;
    math::v2 TextureOffset;
    math::v2 TextureSize;
    b32 IsSolid;
    math::v2 Center;
    collision_AABB CollisionAABB;
    Tile_Layer Layer;
};

#define TILEMAP_LAYERS 5

struct tilemap
{
    i32 Width = 0;
    i32 Height = 0;
    
    i32 TileWidth;
    i32 TileHeight;
    
    i32 TilesheetWidth = 0;
    i32 TilesheetHeight = 0;
    
    char* TextureName;
    
    i32 BufferHandle = 0;
    tilemap_render_info RenderInfo;
    editor_render_info EditorRenderInfo;
    
    tile_data* Tiles;
    u32 TileCount;
    tile_data** Data[TILEMAP_LAYERS];
};

enum Level_Type
{
    Level_Orthogonal,
    Level_Isometric
};

struct level
{
    char* Name;
    char* SheetName;
    Level_Type Type = Level_Orthogonal;
    math::v2 PlayerStartPosition;
    tilemap Tilemap;
    char* TilesheetPath;
    u32 TilesheetIndex;
    i32 AmbientLightHandle;
};


#endif

