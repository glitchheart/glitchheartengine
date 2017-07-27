#ifndef EDITOR_H
#define EDITOR_H

enum Editor_Mode
{
    Editor_Level,
    Editor_Animation,
    Editor_Tilesheet
};

enum Editor_Button
{
    Button_Exit,
    Button_SaveAndExit,
    Button_Tilesheet,
    Button_Animation,
    Button_SwitchMode,
    Button_CreateLevel
};

struct button
{
    b32 Active = false;
    b32 Clicked = false;
    char* Text;
    glm::vec2 ScreenPosition;
    glm::vec2 Size;
    glm::vec4 Color;
    glm::vec4 TextColor;
    timer ClickAnimationTimer;
    
    union
    {
        Editor_Button EditorType;
    };
};

enum Entity_Placement_Type
{
    Placement_Entity_Skeleton,
    Placement_Entity_Blob,
    Placement_Entity_Wraith,
    Placement_Entity_Barrel,
    Placement_Entity_Bonfire,
    Placement_Entity_Minotaur,
    
    Placement_Entity_Max
};

#define TEXTFIELD_LENGTH 30

enum Textfield_Type
{
    Textfield_Normal,
    Textfield_Integer,
    Textfield_Decimal
};

enum Editor_Field_Type
{
    Editor_Animation_Name,
    Editor_Animation_FrameWidth,
    Editor_Animation_FrameHeight,
    Editor_Animation_FrameCount,
    Editor_Animation_FrameOffsetX,
    Editor_Animation_FrameOffsetY,
    Editor_Animation_FrameDuration,
    Editor_Animation_FrameLoopField
};

struct checkbox
{
    b32 Active = false;
    b32 Checked = false;
    char* Label;
    glm::vec2 ScreenPosition;
    glm::vec4 Color;
    b32 JustChecked = false;
};

struct textfield
{
    b32 Active = false;
    b32 InFocus = false;
    Textfield_Type Type = Textfield_Normal;
    char* Label;
    char Text[TEXTFIELD_LENGTH];
    u32 TextIndex;
    glm::vec2 ScreenPosition;
    glm::vec2 Size;
    glm::vec4 Color;
    glm::vec4 TextColor;
};

#define MENU_OPTIONS_COUNT 2

enum Editor_Placement_Mode
{
    Editor_Placement_SelectEntity,
    Editor_Placement_PlaceEntity,
    Editor_Placement_Tile
};

enum Editor_Menu_Option
{
    Editor_Menu_Game,
    Editor_Menu_Level,
    Editor_Menu_Animations
};

#define NUM_EDITOR_BUTTONS = 10

enum Animation_Mode
{
    Animation_SelectAnimation,
    Animation_Edit,
    Animation_SelectTexture,
    Animation_Create
};

struct editor_state
{
    b32 Loaded = false;
    Editor_Mode Mode = Editor_Level;
    Editor_Placement_Mode PlacementMode = Editor_Placement_Tile;
    
    entity* SelectedEntity;
    glm::vec2 CurrentSelectedEntityOffset;
    u32 SelectedTileType = 1;
    glm::vec2 SelectedTilePosition;
    Entity_Placement_Type PlacementEntity;
    
    i32 CurrentTilemapLayer = 1;
    b32 RenderAllLayers = true;
    
    r32 LastKnownMouseX;
    r32 LastKnownMouseY;
    r32 ZoomingSpeed = 50;
    r32 PanningSpeed = 500;
    r32 MinZoom = 5;
    r32 MaxZoom = 100;
    
    r32 TileX;
    r32 TileY;
    
    glm::vec2 TilemapOffset;
    r32 RenderedTileSize = 30.0f;
    
    glm::vec2 TileBrushSize = glm::vec2(1, 1);
    
    r32 ToolbarScrollSpeed = 30000;
    r32 ToolbarScrollOffsetY = 0.0f;
    r32 ToolbarX;
    r32 ToolbarY;
    r32 ToolbarWidth;
    r32 ToolbarHeight;
    
    checkbox Checkboxes[10];
    button Buttons[10];
    textfield Textfields[20];
    i32 FocusedTextfield = -1;
    
    b32 HasLoadedAnimations;
    textfield* AnimationNameField;
    textfield* AnimationFrameWidthField;
    textfield* AnimationFrameHeightField;
    textfield* AnimationFrameCountField;
    textfield* AnimationFrameOffsetXField;
    textfield* AnimationFrameOffsetYField;
    textfield* AnimationFrameDurationField;
    checkbox* AnimationLoopCheckbox;
    checkbox* TileIsSolidCheckbox;
    
    button* CreateNewLevelButton;
    button* CreateNewAnimationButton;
    button* SaveAnimationButton;
    
    textfield* TileBrushWidthField;
    textfield* TileBrushHeightField;
    
    b32 IsInCreateWaypointMode = false;
    
    Animation_Mode AnimationMode = Animation_SelectAnimation;
    b32 Editing;
    b32 ShouldLoop;
    i32 SelectedAnimation = 0;
    animation_info AnimationInfo;
    animation* LoadedAnimation;
    i32 SelectedTexture;
    
    i32 SelectedMenuOption;
    b32 MenuOpen;
    char* MenuOptions[3] = 
    {
        "Game",
        "Level editor",
        "Animations"
    };
};

#endif