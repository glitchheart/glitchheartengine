#ifndef LEVEL_H
#define LEVEL_H

enum Tile_Layer 
{
    Tile_Layer_Background,
    Tile_Layer_Normal
};

struct tile_data
{
    int32 TypeIndex;
    glm::vec2 TextureOffset;
    glm::vec2 TextureSize;
    bool32 IsSolid;
    glm::vec2 Center;
    collision_AABB CollisionAABB;
    Tile_Layer Layer;
};

#define TILEMAP_LAYERS 2

struct tilemap
{
    int32 Width = 0;
    int32 Height = 0;
    uint32 TileSize = 16;
    int32 TilesheetWidth = 0;
    int32 TilesheetHeight = 0;
    
    tilemap_render_info RenderInfo;
    editor_render_info EditorRenderInfo;
    render_entity RenderEntity;
    
    tile_data* Tiles;
    uint32 TileCount;
    tile_data** Data[TILEMAP_LAYERS];
};

struct level
{
    char* Name;
    char* SheetName;
    glm::vec2 PlayerStartPosition;
    tilemap Tilemap;
    char* TilesheetPath;
    uint32 TilesheetIndex;
};


#endif

