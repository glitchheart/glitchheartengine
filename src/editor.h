#ifndef EDITOR_H
#define EDITOR_H

//@Obsolete: Delete this
enum Editor_UI_State
{
    State_Selection,
    State_Animations,
    State_Collision,
    State_EntityList
};

enum Editor_Mode
{
    Editor_Normal,
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
    bool32 Active;
    bool32 Clicked;
    char* Text;
    glm::vec2 ScreenPosition;
    glm::vec2 Size;
    glm::vec4 Color;
    glm::vec4 TextColor;
    timer* ClickAnimationTimer;
    
    union
    {
        Editor_Button EditorType;
    };
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
    bool32 Active;
    bool32 Checked;
    char* Label;
    glm::vec2 ScreenPosition;
    glm::vec4 Color;
    bool32 JustChecked;
};

struct textfield
{
    bool32 Active;
    bool32 InFocus;
    Textfield_Type Type;
    char* Label;
    char Text[TEXTFIELD_LENGTH];
    uint32 TextIndex;
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

#define NUM_EDITOR_BUTTONS = 10

struct editor_state
{
    bool32 Loaded;
    Editor_Mode Mode = Editor_Normal;
    Editor_Placement_Mode PlacementMode = Editor_Placement_Tile;
    
    entity* SelectedEntity;
    uint32 SelectedTileType;
    entity* PlacementEntity;
    
    real32 LastKnownMouseX;
    real32 LastKnownMouseY;
    real32 ZoomingSpeed;
    real32 PanningSpeed;
    real32 MinZoom;
    real32 MaxZoom;
    
    real32 TileX;
    real32 TileY;
    
    real32 ToolbarScrollSpeed;
    real32 ToolbarScrollOffsetY;
    real32 ToolbarX;
    real32 ToolbarY;
    real32 ToolbarWidth;
    real32 ToolbarHeight;
    
    checkbox Checkboxes[10];
    button Buttons[10];
    textfield Textfields[20];
    int32 FocusedTextfield;
    
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
    
    bool32 Editing;
    bool32 ShouldLoop;
    int32 SelectedAnimation;
    animation_info AnimationInfo;
    animation* LoadedAnimation;
    int32 SelectedTexture;
    char const** Textures;
    int32 TexturesLength;
    char** Animations;
    int32 AnimationsLength;
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