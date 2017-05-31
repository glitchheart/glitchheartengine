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

struct editor_state
{
    entity* SelectedEntity;
    real32 LastKnownMouseX;
    real32 LastKnownMouseY;
    real32 ZoomingSpeed = 12;
    real32 PanningSpeed = 6;
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