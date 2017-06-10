#ifndef EDITOR_H
#define EDITOR_H

enum Editor_UI_State
{
    State_Selection,
    State_Animations,
    State_Collision,
    State_EntityList
};

#define MENU_OPTIONS_COUNT 2

enum Editor_Placement_Mode
{
    Editor_Placement_Entity,
    Editor_Placement_Tile
};

struct editor_state
{
    Editor_Placement_Mode PlacementMode = Editor_Placement_Tile;
    
    entity* SelectedEntity;
    uint32 SelectedTileType = 1;
    
    real32 LastKnownMouseX;
    real32 LastKnownMouseY;
    real32 ZoomingSpeed = 50;
    real32 PanningSpeed = 30;
    real32 MinZoom = 0.1f;
    real32 MaxZoom = 10;
    
    real32 TileX;
    real32 TileY;
    
    real32 ToolbarScrollSpeed = 30000;
    real32 ToolbarScrollOffsetY = 0.0f;
    real32 ToolbarX;
    real32 ToolbarY;
    real32 ToolbarWidth;
    real32 ToolbarHeight;
};

struct editor_ui
{
    bool32 On;
    Editor_UI_State State;
    uint32 SelectedIndex;
    char* MenuOptions[2] = 
    {
        "Animations",
        "Collision",
    };
};

#endif