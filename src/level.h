#ifndef LEVEL_H
#define LEVEL_H

enum Tile_Layer //TODO(Daniel) use this for making different tile layers later
{
    Tile_Layer_None
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

struct tilemap
{
    uint32 Width;
    uint32 Height;
    uint32 TileSize;
    tilemap_render_info RenderInfo;
    editor_render_info EditorRenderInfo;
    render_entity RenderEntity;
    
    tile_data* Tiles;
    uint32 TileCount;
    tile_data** Data;
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

